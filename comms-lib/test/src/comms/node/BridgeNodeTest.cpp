#define BOOST_TEST_DYN_LINK
#ifdef STAND_ALONE
#   define BOOST_TEST_MODULE Main
#endif
#include <boost/test/unit_test.hpp>
#include <fakeit.hpp>

#include "comms/node/BridgeNode.h"


namespace BridgeNodeTest {

    /*
     * Helper fixture for initializing and preparing all of the necessary mocks
     */
    struct SetupMocks {

            SetupMocks() : internalConnMsgListener(NULL), externalConnMsgListener(NULL) {
                fakeit::When(Method(mockInternalConnection, addMessageListener)).AlwaysDo([this](cadf::comms::IMessageListener *listener) {
                    internalConnMsgListener = listener;
                });
                fakeit::When(Method(mockInternalConnection, removeMessageListener)).AlwaysReturn();
                fakeit::When(Method(mockInternalConnection, connect)).AlwaysReturn(true);
                fakeit::When(Method(mockInternalConnection, disconnect)).AlwaysReturn(true);
                fakeit::When(Method(mockInternalConnection, sendMessage)).AlwaysReturn(true);

                fakeit::When(Method(mockExternalConnection, addMessageListener)).AlwaysDo([this](cadf::comms::IMessageListener *listener) {
                    externalConnMsgListener = listener;
                });
                fakeit::When(Method(mockExternalConnection, removeMessageListener)).AlwaysReturn();
                fakeit::When(Method(mockExternalConnection, connect)).AlwaysReturn(true);
                fakeit::When(Method(mockExternalConnection, disconnect)).AlwaysReturn(true);
                fakeit::When(Method(mockExternalConnection, sendMessage)).AlwaysReturn(true);

                fakeit::When(Method(mockPacket1, getMessage)).AlwaysReturn(&mockMessage1.get());
                fakeit::When(Method(mockPacket2, getMessage)).AlwaysReturn(&mockMessage2.get());
                fakeit::When(Method(mockPacket3, getMessage)).AlwaysReturn(&mockMessage3.get());
                fakeit::When(Method(mockPacket4, getMessage)).AlwaysReturn(&mockMessage4.get());
                fakeit::When(Method(mockPacket5, getMessage)).AlwaysReturn(&mockMessage5.get());
                fakeit::When(Method(mockPacket6, getMessage)).AlwaysReturn(&mockMessage6.get());

                fakeit::When(Method(mockMessage1, getType)).AlwaysReturn("Message1");
                fakeit::When(Method(mockMessage2, getType)).AlwaysReturn("Message2");
                fakeit::When(Method(mockMessage3, getType)).AlwaysReturn("Message3");
                fakeit::When(Method(mockMessage4, getType)).AlwaysReturn("Message4");
                fakeit::When(Method(mockMessage5, getType)).AlwaysReturn("Message5");
                fakeit::When(Method(mockMessage6, getType)).AlwaysReturn("Message6");
            }

            ~SetupMocks() {
                verifyAllMocksChecked();
            }

            void verifyAllMocksChecked() {
                fakeit::VerifyNoOtherInvocations(mockInternalConnection, mockExternalConnection, mockMessage1, mockMessage2, mockMessage3, mockMessage4, mockMessage5, mockMessage6);
            }

            // Mocks to use for testing
            fakeit::Mock<cadf::comms::IConnection> mockInternalConnection;
            fakeit::Mock<cadf::comms::IConnection> mockExternalConnection;
            fakeit::Mock<cadf::comms::MessagePacket> mockPacket1;
            fakeit::Mock<cadf::comms::MessagePacket> mockPacket2;
            fakeit::Mock<cadf::comms::MessagePacket> mockPacket3;
            fakeit::Mock<cadf::comms::MessagePacket> mockPacket4;
            fakeit::Mock<cadf::comms::MessagePacket> mockPacket5;
            fakeit::Mock<cadf::comms::MessagePacket> mockPacket6;
            fakeit::Mock<cadf::comms::IMessage> mockMessage1;
            fakeit::Mock<cadf::comms::IMessage> mockMessage2;
            fakeit::Mock<cadf::comms::IMessage> mockMessage3;
            fakeit::Mock<cadf::comms::IMessage> mockMessage4;
            fakeit::Mock<cadf::comms::IMessage> mockMessage5;
            fakeit::Mock<cadf::comms::IMessage> mockMessage6;

            // Listeners that are assigned to the connections
            cadf::comms::IMessageListener *internalConnMsgListener;
            cadf::comms::IMessageListener *externalConnMsgListener;
    };

    /**
     * Helper fixture to initialize the BridgeNode instance that is to be used for the tests
     */
    struct TestFixture: public SetupMocks {

            TestFixture() : SetupMocks(), bridge(new cadf::comms::BridgeNode(&mockInternalConnection.get(), &mockExternalConnection.get())) {
                // Verify that on initializing the node the connection is established
                fakeit::Verify(Method(mockInternalConnection, addMessageListener)).Once();
                fakeit::Verify(Method(mockInternalConnection, connect)).Once();
                fakeit::Verify(Method(mockExternalConnection, addMessageListener)).Once();
                fakeit::Verify(Method(mockExternalConnection, connect)).Once();
                verifyAllMocksChecked();
            }

            ~TestFixture() {
                delete (bridge);
                fakeit::Verify(Method(mockInternalConnection, removeMessageListener)).Once();
                fakeit::Verify(Method(mockInternalConnection, disconnect)).Once();
                fakeit::Verify(Method(mockExternalConnection, removeMessageListener)).Once();
                fakeit::Verify(Method(mockExternalConnection, disconnect)).Once();
                verifyAllMocksChecked();
            }

            // The class to test
            cadf::comms::BridgeNode *bridge;
    };

}

BOOST_AUTO_TEST_SUITE(BridgeNode_Test_Suite)

/**
 * If no forwarding rules have been applied, then messages will not be forwarded
 */
    BOOST_FIXTURE_TEST_CASE(SendMessageNoForwardingSetupTest, BridgeNodeTest::TestFixture) {
        // Message received from external
        externalConnMsgListener->messageReceived(&mockPacket1.get());
        fakeit::Verify(Method(mockPacket1, getMessage)).Once();
        fakeit::Verify(Method(mockMessage1, getType)).Once();
        verifyAllMocksChecked();

        // Message received from internal
        internalConnMsgListener->messageReceived(&mockPacket2.get());
        fakeit::Verify(Method(mockPacket2, getMessage)).Once();
        fakeit::Verify(Method(mockMessage2, getType)).Once();
        verifyAllMocksChecked();
    }

    /**
     * Verify that messages from external can be forwarded to the internal, based on specified rules
     */
    BOOST_FIXTURE_TEST_CASE(ForwardMessageFromExternalWithRulesTest, BridgeNodeTest::TestFixture) {
        // Specify some rules
        bridge->addForwardToInternalRule("Message1", 1, 1);
        bridge->addForwardToInternalRule("Message2", -1, -1);
        bridge->addForwardToInternalRule("Message3", 123, -1);
        bridge->addForwardToInternalRule("Message4", -1, 456);
        bridge->addForwardToInternalRule("Message5", 135, 79);

        // Message1 received from external
        externalConnMsgListener->messageReceived(&mockPacket1.get());
        fakeit::Verify(Method(mockPacket1, getMessage)).Once();
        fakeit::Verify(Method(mockMessage1, getType)).Once();
        fakeit::Verify(Method(mockInternalConnection, sendMessage).Using(&mockMessage1.get(), 1, 1)).Once();
        verifyAllMocksChecked();

        // Message2 received from external
        externalConnMsgListener->messageReceived(&mockPacket2.get());
        fakeit::Verify(Method(mockPacket2, getMessage)).Once();
        fakeit::Verify(Method(mockMessage2, getType)).Once();
        fakeit::Verify(Method(mockInternalConnection, sendMessage).Using(&mockMessage2.get(), -1, -1)).Once();
        verifyAllMocksChecked();

        // Message3 received from external
        externalConnMsgListener->messageReceived(&mockPacket3.get());
        fakeit::Verify(Method(mockPacket3, getMessage)).Once();
        fakeit::Verify(Method(mockMessage3, getType)).Once();
        fakeit::Verify(Method(mockInternalConnection, sendMessage).Using(&mockMessage3.get(), 123, -1)).Once();
        verifyAllMocksChecked();

        // Message4 received from external
        externalConnMsgListener->messageReceived(&mockPacket4.get());
        fakeit::Verify(Method(mockPacket4, getMessage)).Once();
        fakeit::Verify(Method(mockMessage4, getType)).Once();
        fakeit::Verify(Method(mockInternalConnection, sendMessage).Using(&mockMessage4.get(), -1, 456)).Once();
        verifyAllMocksChecked();

        // Message5 received from external
        externalConnMsgListener->messageReceived(&mockPacket5.get());
        fakeit::Verify(Method(mockPacket5, getMessage)).Once();
        fakeit::Verify(Method(mockMessage5, getType)).Once();
        fakeit::Verify(Method(mockInternalConnection, sendMessage).Using(&mockMessage5.get(), 135, 79)).Once();
        verifyAllMocksChecked();

        // Message6 received from external
        externalConnMsgListener->messageReceived(&mockPacket6.get());
        fakeit::Verify(Method(mockPacket6, getMessage)).Once();
        fakeit::Verify(Method(mockMessage6, getType)).Once();
        // No routing for Message6, therefore not forwarded
        verifyAllMocksChecked();
    }

    /**
     * Verify that messages from internal can be forwarded to the external, based on specified rules
     */
    BOOST_FIXTURE_TEST_CASE(ForwardMessageFromInternalWithRulesTest, BridgeNodeTest::TestFixture) {
        // Specify some rules
        bridge->addForwardToExternalRule("Message1", -1, -1);
        bridge->addForwardToExternalRule("Message2", 135, -1);
        bridge->addForwardToExternalRule("Message3", -1, 79);
        bridge->addForwardToExternalRule("Message6", 123, 456);

        // Message1 received from internal
        internalConnMsgListener->messageReceived(&mockPacket1.get());
        fakeit::Verify(Method(mockPacket1, getMessage)).Once();
        fakeit::Verify(Method(mockMessage1, getType)).Once();
        fakeit::Verify(Method(mockExternalConnection, sendMessage).Using(&mockMessage1.get(), -1, -1)).Once();
        verifyAllMocksChecked();

        // Message2 received from internal
        internalConnMsgListener->messageReceived(&mockPacket2.get());
        fakeit::Verify(Method(mockPacket2, getMessage)).Once();
        fakeit::Verify(Method(mockMessage2, getType)).Once();
        fakeit::Verify(Method(mockExternalConnection, sendMessage).Using(&mockMessage2.get(), 135, -1)).Once();
        verifyAllMocksChecked();

        // Message3 received from internal
        internalConnMsgListener->messageReceived(&mockPacket3.get());
        fakeit::Verify(Method(mockPacket3, getMessage)).Once();
        fakeit::Verify(Method(mockMessage3, getType)).Once();
        fakeit::Verify(Method(mockExternalConnection, sendMessage).Using(&mockMessage3.get(), -1, 79)).Once();
        verifyAllMocksChecked();

        // Message4 received from internal
        internalConnMsgListener->messageReceived(&mockPacket4.get());
        fakeit::Verify(Method(mockPacket4, getMessage)).Once();
        fakeit::Verify(Method(mockMessage4, getType)).Once();
        // No routing for Message4, therefore not forwarded
        verifyAllMocksChecked();

        // Message5 received from internal
        internalConnMsgListener->messageReceived(&mockPacket5.get());
        fakeit::Verify(Method(mockPacket5, getMessage)).Once();
        fakeit::Verify(Method(mockMessage5, getType)).Once();
        // No routing for Message5, therefore not forwarded
        verifyAllMocksChecked();

        // Message6 received from internal
        internalConnMsgListener->messageReceived(&mockPacket6.get());
        fakeit::Verify(Method(mockPacket6, getMessage)).Once();
        fakeit::Verify(Method(mockMessage6, getType)).Once();
        fakeit::Verify(Method(mockExternalConnection, sendMessage).Using(&mockMessage6.get(), 123, 456)).Once();
        verifyAllMocksChecked();
    }

    BOOST_AUTO_TEST_SUITE_END()
