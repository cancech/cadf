
#define BOOST_TEST_DYN_LINK
#ifdef STAND_ALONE
#   define BOOST_TEST_MODULE Main
#endif
#include <boost/test/unit_test.hpp>
#include <fakeit.hpp>

#include "comms/connection/ClientConnection.h"
#include "comms/network/serializer/dom/JsonSerializer.h"

#include "TestMessage.h"

// Helper classes for the ClientConnectionTest
namespace ClientConnectionTest {
    /*
     * Helper fixture for initializing and preparing all of the necessary mocks
     */
    struct SetupMocks {

            SetupMocks() {
                fakeit::Fake(Method(mockClient, setListener));
                fakeit::When(Method(mockClient, isConnected)).AlwaysReturn(false);
                fakeit::When(Method(mockClient, connect)).AlwaysReturn(true);
                fakeit::When(Method(mockClient, disconnect)).AlwaysReturn(true);
                fakeit::When(Method(mockClient, send)).AlwaysReturn(true);
                fakeit::Fake(Method(mockListener, messageReceived));

                fakeit::When(Method(mockFactory, serializeMessage)).AlwaysDo([&](const cadf::comms::MessagePacket &packet) {
                    lastUsedOutBuffer = new cadf::comms::OutputBuffer(1);
                    return lastUsedOutBuffer;
                });
            }

            ~SetupMocks() {
                verifyAllMocksChecked();
            }

            void verifyAllMocksChecked() {
                fakeit::VerifyNoOtherInvocations(mockClient, mockListener, mockFactory, mockInBuffer);
            }

            // Classes to support mocking
            cadf::comms::OutputBuffer *lastUsedOutBuffer = NULL;

            // Mocks to use for testing
            fakeit::Mock<cadf::comms::IClient> mockClient;
            fakeit::Mock<cadf::comms::IMessageListener> mockListener;
            fakeit::Mock<cadf::comms::MessageFactory<cadf::comms::dom::json::JSONProtocol>> mockFactory;
            fakeit::Mock<cadf::comms::InputBuffer> mockInBuffer;
    };

    /**
     * Helper fixture to initialize the ClientConnection instance that is to be used for the tests
     */
    struct TestFixture: public SetupMocks {

            TestFixture() : SetupMocks(), conn(948, 183, &mockFactory.get(), &mockClient.get()) {
                BOOST_CHECK_EQUAL(948, conn.getType());
                BOOST_CHECK_EQUAL(183, conn.getInstance());

                // Verify the initialization of the class to be tested
                fakeit::Verify(Method(mockClient, setListener).Using(&conn)).Once();
                verifyAllMocksChecked();

                conn.addMessageListener(&mockListener.get());
                verifyAllMocksChecked();
            }

            void connect(int timesConnected) {
                BOOST_CHECK(conn.connect());
                fakeit::Verify(Method(mockClient, connect)).Exactly(timesConnected);
                fakeit::When(Method(mockClient, isConnected)).AlwaysReturn(true);
            }

            void disconnect(int timesDisconnected) {
                BOOST_CHECK(conn.disconnect());
                fakeit::Verify(Method(mockClient, disconnect)).Exactly(timesDisconnected);
                fakeit::When(Method(mockClient, isConnected)).AlwaysReturn(false);
            }

            // The class to test
            cadf::comms::ClientConnection<cadf::comms::dom::json::JSONProtocol> conn;
    };
}

/**
 * Unit test for the ClientConnection
 */
BOOST_AUTO_TEST_SUITE(ClientConnection_Test_Suite)

/**
 * Verify that can create a ClientConnection, and can connect and disconnect
 */
    BOOST_FIXTURE_TEST_CASE(CycleConnectionTest, ClientConnectionTest::TestFixture) {
        BOOST_CHECK_EQUAL(false, conn.isConnected());
        fakeit::Verify(Method(mockClient, isConnected)).Exactly(1);
        connect(1);
        BOOST_CHECK_EQUAL(true, conn.isConnected());
        fakeit::Verify(Method(mockClient, isConnected)).Exactly(2);
        disconnect(1);
        BOOST_CHECK_EQUAL(false, conn.isConnected());
        fakeit::Verify(Method(mockClient, isConnected)).Exactly(3);

        verifyAllMocksChecked();
    }

    /**
     * Verify that cannot send any messages when disconnected.
     */
    BOOST_FIXTURE_TEST_CASE(CannotSendWhenDisconnectedTest, ClientConnectionTest::TestFixture) {
        BOOST_CHECK_EQUAL(false, conn.isConnected());
        fakeit::Verify(Method(mockClient, isConnected)).Exactly(1);

        BOOST_CHECK_EQUAL(false, conn.sendMessage(NULL));
        fakeit::Verify(Method(mockClient, isConnected)).Exactly(2);
        BOOST_CHECK_EQUAL(false, conn.sendPacket(NULL));
        fakeit::Verify(Method(mockClient, isConnected)).Exactly(3);
    }

    /**
     * Verify that can send messages when connected.
     */
    BOOST_FIXTURE_TEST_CASE(SendWhenConnectedTest, ClientConnectionTest::TestFixture) {
        BOOST_CHECK_EQUAL(false, conn.isConnected());
        fakeit::Verify(Method(mockClient, isConnected)).Exactly(1);
        connect(1);
        BOOST_CHECK_EQUAL(true, conn.isConnected());
        fakeit::Verify(Method(mockClient, isConnected)).Exactly(2);

        // Send the message
        TestMessage1 msg;
        BOOST_CHECK_EQUAL(true, conn.sendMessage(&msg));
        fakeit::Verify(Method(mockClient, isConnected)).Exactly(4);
        fakeit::Verify(Method(mockFactory, serializeMessage)).Exactly(1);
        fakeit::Verify(Method(mockClient, send).Using(lastUsedOutBuffer)).Exactly(1);

        // Send the packet
        TestMessage2 msg2;
        cadf::comms::MessagePacket packet(&msg2, 1, 2);
        BOOST_CHECK_EQUAL(true, conn.sendPacket(&packet));
        fakeit::Verify(Method(mockClient, isConnected)).Exactly(5);
        fakeit::Verify(Method(mockFactory, serializeMessage)).Exactly(2);
        fakeit::Verify(Method(mockClient, send).Using(lastUsedOutBuffer)).Exactly(1);
    }

    /**
     * Verify that can receive messages when connected.
     */
    BOOST_FIXTURE_TEST_CASE(ReceiveMessageTest, ClientConnectionTest::TestFixture) {
        cadf::comms::MessagePacket *packet = new cadf::comms::MessagePacket(new TestMessage1(), -1, -1);
        fakeit::When(Method(mockFactory, deserializeMessage)).AlwaysReturn(packet);

        conn.messageReceived(&mockInBuffer.get());
        fakeit::Verify(Method(mockFactory, deserializeMessage).Using(&mockInBuffer.get())).Once();
        fakeit::Verify(Method(mockListener, messageReceived).Using(packet)).Once();
    }

    /**
     * Verify that properly processes the handshake message
     */
    BOOST_FIXTURE_TEST_CASE(ReceiveHandshakeMessageTest, ClientConnectionTest::TestFixture) {
        // Make sure that the connection is connected
        connect(1);

        // Prepare the custom mocks required for this test
        cadf::comms::MessagePacket *packet = new cadf::comms::MessagePacket(new cadf::comms::HandshakeInitMessage(), -1, -1);
        fakeit::When(Method(mockFactory, deserializeMessage)).AlwaysReturn(packet);

        std::string sentMsgType = "";
        fakeit::When(Method(mockFactory, serializeMessage)).AlwaysDo([&](const cadf::comms::MessagePacket &packet) {
            sentMsgType = packet.getMessage()->getType();
            lastUsedOutBuffer = new cadf::comms::OutputBuffer(1);
            return lastUsedOutBuffer;
        });

        conn.messageReceived(&mockInBuffer.get());
        fakeit::Verify(Method(mockClient, isConnected)).Exactly(2);
        fakeit::Verify(Method(mockFactory, deserializeMessage).Using(&mockInBuffer.get())).Once();
        fakeit::Verify(Method(mockFactory, serializeMessage)).Once();
        BOOST_CHECK_EQUAL("HandshakeResponseMessageV1", sentMsgType);
        fakeit::Verify(Method(mockClient, send)).Exactly(1);
        fakeit::Verify(Method(mockListener, messageReceived)).Never();
    }

    BOOST_AUTO_TEST_SUITE_END()
