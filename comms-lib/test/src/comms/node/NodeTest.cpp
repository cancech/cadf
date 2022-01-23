#define BOOST_TEST_DYN_LINK
#ifdef STAND_ALONE
#   define BOOST_TEST_MODULE Main
#endif
#include <boost/test/unit_test.hpp>

#include <comms/node/Node.h>

#include <fakeit.hpp>

// Helper classes for the NodeTest
namespace NodeTest {

    /*
     * Extension of the Node to expose protected methods such that the unit test can call them freely.
     */
    class NodeTester: public cadf::comms::Node {

        public:
            NodeTester(cadf::comms::IConnection *connection) : cadf::comms::Node(connection) {
            }

            // Expose the messageReceived method such that the unit test can call it directly
            virtual void messageReceived(const cadf::comms::MessagePacket *packet) {
                cadf::comms::Node::messageReceived(packet);
            }
    };

    /*
     * Helper fixture for initializing and preparing all of the necessary mocks
     */
    struct SetupMocks {

            SetupMocks() {
                fakeit::When(Method(mockConnection, addMessageListener)).AlwaysReturn();
                fakeit::When(Method(mockConnection, removeMessageListener)).AlwaysReturn();
                fakeit::When(Method(mockConnection, connect)).AlwaysReturn(true);
                fakeit::When(Method(mockConnection, disconnect)).AlwaysReturn(true);
                fakeit::When(Method(mockConnection, isConnected)).AlwaysReturn(false);
                fakeit::When(Method(mockConnection, sendMessage)).AlwaysReturn(true);

                fakeit::When(Method(mockSomeProcessor, getType)).AlwaysReturn("SomeMessage");
                fakeit::When(Method(mockSomeProcessor, process)).AlwaysReturn();
                fakeit::When(Method(mockOtherProcessor, getType)).AlwaysReturn("OtherMessage");
                fakeit::When(Method(mockOtherProcessor, process)).AlwaysReturn();

                fakeit::When(Method(mockSomeMessagePacket, getMessage)).AlwaysReturn(&mockSomeMessage.get());
                fakeit::When(Method(mockOtherMessagePacket, getMessage)).AlwaysReturn(&mockOtherMessage.get());

                fakeit::When(Method(mockSomeMessage, getType)).AlwaysReturn("SomeMessage");
                fakeit::When(Method(mockOtherMessage, getType)).AlwaysReturn("OtherMessage");
            }

            ~SetupMocks() {
                verifyAllMocksChecked();
            }

            void verifyAllMocksChecked() {
                fakeit::VerifyNoOtherInvocations(mockConnection, mockSomeProcessor, mockOtherProcessor, mockSomeMessage, mockOtherMessage);
            }

            // Mocks to use for testing
            fakeit::Mock<cadf::comms::IConnection> mockConnection;
            fakeit::Mock<cadf::comms::IProcessor> mockSomeProcessor;
            fakeit::Mock<cadf::comms::IProcessor> mockOtherProcessor;
            fakeit::Mock<cadf::comms::MessagePacket> mockSomeMessagePacket;
            fakeit::Mock<cadf::comms::MessagePacket> mockOtherMessagePacket;
            fakeit::Mock<cadf::comms::IMessage> mockSomeMessage;
            fakeit::Mock<cadf::comms::IMessage> mockOtherMessage;
    };

    /**
     * Helper fixture to initialize the LocalConnection instance that is to be used for the tests
     */
    struct TestFixture: public SetupMocks {

            TestFixture() : SetupMocks(), node(new NodeTester(&mockConnection.get())) {
                // Verify that on initializing the node the connection is established
                fakeit::Verify(Method(mockConnection, addMessageListener).Using(node)).Once();
                verifyAllMocksChecked();
            }

            ~TestFixture() {
                delete (node);
                fakeit::Verify(Method(mockConnection, removeMessageListener).Using(node)).Once();
                fakeit::Verify(Method(mockConnection, isConnected)).AtLeast(++timesIsConnected);

                if (isConnected)
                    fakeit::Verify(Method(mockConnection, disconnect)).Once();

                verifyAllMocksChecked();
            }

            void connectNode(int timesConnect) {
                BOOST_CHECK(node->connect());
                fakeit::Verify(Method(mockConnection, isConnected)).Exactly(++timesIsConnected);
                fakeit::Verify(Method(mockConnection, connect)).Exactly(timesConnect);
                fakeit::When(Method(mockConnection, isConnected)).AlwaysReturn(true);
                isConnected = true;
            }

            void disconnectNode(int timesDisconnect) {
                BOOST_CHECK(node->disconnect());
                fakeit::Verify(Method(mockConnection, isConnected)).Exactly(++timesIsConnected);
                fakeit::Verify(Method(mockConnection, disconnect)).Exactly(timesDisconnect);
                fakeit::When(Method(mockConnection, isConnected)).AlwaysReturn(false);
                isConnected = false;
            }

            bool isConnected = false;
            int timesIsConnected = 0;

            // The class to test
            NodeTester *node;
    };

}

BOOST_AUTO_TEST_SUITE(Node_Test_Suite)

/**
 * Verify that the node can connected when it was disconnected
 */
    BOOST_FIXTURE_TEST_CASE(ConnectWhenDisconnectedTest, NodeTest::TestFixture) {
        BOOST_CHECK(!node->isConnected());
        fakeit::Verify(Method(mockConnection, isConnected)).Exactly(++timesIsConnected);

        connectNode(1);

        BOOST_CHECK(node->isConnected());
        fakeit::Verify(Method(mockConnection, isConnected)).Exactly(++timesIsConnected);
    }

    /**
     * Verify that nothing happens when already connected
     */
    BOOST_FIXTURE_TEST_CASE(ConnectWhenConnectedTest, ConnectWhenDisconnectedTest) {
        ConnectWhenDisconnectedTest::test_method();

        connectNode(1);
        BOOST_CHECK(node->isConnected());
        fakeit::Verify(Method(mockConnection, isConnected)).Exactly(++timesIsConnected);

        connectNode(1);
        BOOST_CHECK(node->isConnected());
        fakeit::Verify(Method(mockConnection, isConnected)).Exactly(++timesIsConnected);

        connectNode(1);
        BOOST_CHECK(node->isConnected());
        fakeit::Verify(Method(mockConnection, isConnected)).Exactly(++timesIsConnected);
    }

    /**
     * Verify that can disconnect after connecting
     */
    BOOST_FIXTURE_TEST_CASE(DisonnectWhenConnectedTest, ConnectWhenConnectedTest) {
        ConnectWhenConnectedTest::test_method();

        disconnectNode(1);
        BOOST_CHECK(!node->isConnected());
        fakeit::Verify(Method(mockConnection, isConnected)).Exactly(++timesIsConnected);
    }

    /**
     * Verify that disconnecting when disconnected does nothing
     */
    BOOST_FIXTURE_TEST_CASE(DisonnectWhenDisconnectedTest, DisonnectWhenConnectedTest) {
        DisonnectWhenConnectedTest::test_method();

        disconnectNode(1);
        BOOST_CHECK(!node->isConnected());
        fakeit::Verify(Method(mockConnection, isConnected)).Exactly(++timesIsConnected);

        disconnectNode(1);
        BOOST_CHECK(!node->isConnected());
        fakeit::Verify(Method(mockConnection, isConnected)).Exactly(++timesIsConnected);

        disconnectNode(1);
        BOOST_CHECK(!node->isConnected());
        fakeit::Verify(Method(mockConnection, isConnected)).Exactly(++timesIsConnected);
    }

    /**
     * Verify that the node can send a message when connected
     */
    BOOST_FIXTURE_TEST_CASE(SendMessageWhenConnectedTest, ConnectWhenDisconnectedTest) {
        ConnectWhenDisconnectedTest::test_method();

        // Mocked connection returns true
        BOOST_CHECK(node->sendMessage(&mockSomeMessage.get(), 1, 2));
        fakeit::Verify(Method(mockConnection, isConnected)).Exactly(++timesIsConnected);
        fakeit::Verify(Method(mockConnection, sendMessage).Using(&mockSomeMessage.get(), 1, 2)).Once();
        verifyAllMocksChecked();
    }

    /**
     * Verify that the node cannot send a message when disconnected
     */
    BOOST_FIXTURE_TEST_CASE(SendMessageWhenDisconnectedTest, NodeTest::TestFixture) {
        BOOST_CHECK_EQUAL(false, node->sendMessage(&mockOtherMessage.get(), 2, 1));
        fakeit::Verify(Method(mockConnection, isConnected)).Exactly(++timesIsConnected);
        verifyAllMocksChecked();
    }

    /**
     * Verify that nothing happens if there is no processor registered to process received messages
     */
    BOOST_FIXTURE_TEST_CASE(MessageReceivedNoProcessorTest, NodeTest::TestFixture) {
        node->messageReceived(&mockSomeMessagePacket.get());
        fakeit::Verify(Method(mockSomeMessagePacket, getMessage)).Once();
        fakeit::Verify(Method(mockSomeMessage, getType)).Once();
        verifyAllMocksChecked();

        node->messageReceived(&mockOtherMessagePacket.get());
        fakeit::Verify(Method(mockOtherMessagePacket, getMessage)).Once();
        fakeit::Verify(Method(mockOtherMessage, getType)).Once();
        verifyAllMocksChecked();
    }

    /**
     * Verify that with a single processor registered, that it is the one that is told to process a message
     */
    BOOST_FIXTURE_TEST_CASE(SomeMessageProcessorRegisteredTest, NodeTest::TestFixture) {
        // Register the processor
        node->addProcessor(&mockSomeProcessor.get());
        fakeit::Verify(Method(mockSomeProcessor, getType)).Once();
        verifyAllMocksChecked();

        // Since the processor is registered, it is called
        node->messageReceived(&mockSomeMessagePacket.get());
        fakeit::Verify(Method(mockSomeMessagePacket, getMessage)).Once();
        fakeit::Verify(Method(mockSomeMessage, getType)).Once();
        fakeit::Verify(Method(mockSomeProcessor, process).Using(&mockSomeMessage.get())).Once();
        verifyAllMocksChecked();

        // The processor is not registered, therefore nothing happens
        node->messageReceived(&mockOtherMessagePacket.get());
        fakeit::Verify(Method(mockOtherMessagePacket, getMessage)).Once();
        fakeit::Verify(Method(mockOtherMessage, getType)).Once();
        verifyAllMocksChecked();
    }

    /**
     * Verify that with a single processor registered, that it is the one that is told to process a message
     */
    BOOST_FIXTURE_TEST_CASE(OtherMessageProcessorRegisteredTest, NodeTest::TestFixture) {
        // Register the processor
        node->addProcessor(&mockOtherProcessor.get());
        fakeit::Verify(Method(mockOtherProcessor, getType)).Once();
        verifyAllMocksChecked();

        // The processor is not registered, therefore nothing happens
        node->messageReceived(&mockSomeMessagePacket.get());
        fakeit::Verify(Method(mockSomeMessagePacket, getMessage)).Once();
        fakeit::Verify(Method(mockSomeMessage, getType)).Once();
        verifyAllMocksChecked();

        // Since the processor is registered, it is called
        node->messageReceived(&mockOtherMessagePacket.get());
        fakeit::Verify(Method(mockOtherMessagePacket, getMessage)).Once();
        fakeit::Verify(Method(mockOtherMessage, getType)).Once();
        fakeit::Verify(Method(mockOtherProcessor, process).Using(&mockOtherMessage.get())).Once();
        verifyAllMocksChecked();
    }

    /**
     * Verify that both message processors can be registered and processing messages, but when removed the cease processing.
     */
    BOOST_FIXTURE_TEST_CASE(BothProcessorsRegisteredThenSomeProcessorRemovedTest, NodeTest::TestFixture) {
        // Register the processors
        node->addProcessor(&mockOtherProcessor.get());
        fakeit::Verify(Method(mockOtherProcessor, getType)).Once();
        verifyAllMocksChecked();
        node->addProcessor(&mockSomeProcessor.get());
        fakeit::Verify(Method(mockSomeProcessor, getType)).Once();
        verifyAllMocksChecked();

        // With both processors registered, both messages are processed.
        node->messageReceived(&mockSomeMessagePacket.get());
        fakeit::Verify(Method(mockSomeMessagePacket, getMessage)).Once();
        fakeit::Verify(Method(mockSomeMessage, getType)).Once();
        fakeit::Verify(Method(mockSomeProcessor, process).Using(&mockSomeMessage.get())).Once();
        node->messageReceived(&mockOtherMessagePacket.get());
        fakeit::Verify(Method(mockOtherMessagePacket, getMessage)).Once();
        fakeit::Verify(Method(mockOtherMessage, getType)).Once();
        fakeit::Verify(Method(mockOtherProcessor, process).Using(&mockOtherMessage.get())).Once();
        verifyAllMocksChecked();

        // Remove a processor
        node->removeProcessor(&mockSomeProcessor.get());
        fakeit::Verify(Method(mockSomeProcessor, getType)).Exactly(2);
        verifyAllMocksChecked();

        // Resend the messages, but only one is processed
        node->messageReceived(&mockSomeMessagePacket.get());
        fakeit::Verify(Method(mockSomeMessagePacket, getMessage)).Exactly(2);
        fakeit::Verify(Method(mockSomeMessage, getType)).Exactly(2);
        node->messageReceived(&mockOtherMessagePacket.get());
        fakeit::Verify(Method(mockOtherMessagePacket, getMessage)).Exactly(2);
        fakeit::Verify(Method(mockOtherMessage, getType)).Exactly(2);
        fakeit::Verify(Method(mockOtherProcessor, process).Using(&mockOtherMessage.get())).Exactly(2);
        verifyAllMocksChecked();

        // Remove the other processor
        node->removeProcessor(&mockOtherProcessor.get());
        fakeit::Verify(Method(mockOtherProcessor, getType)).Exactly(2);
        verifyAllMocksChecked();

        // Resend the messages, none are processed
        node->messageReceived(&mockSomeMessagePacket.get());
        fakeit::Verify(Method(mockSomeMessagePacket, getMessage)).Exactly(3);
        fakeit::Verify(Method(mockSomeMessage, getType)).Exactly(3);
        node->messageReceived(&mockOtherMessagePacket.get());
        fakeit::Verify(Method(mockOtherMessagePacket, getMessage)).Exactly(3);
        fakeit::Verify(Method(mockOtherMessage, getType)).Exactly(3);
        verifyAllMocksChecked();
    }

    /**
     * Verify that both message processors can be registered and processing messages, but when removed the cease processing.
     */
    BOOST_FIXTURE_TEST_CASE(BothProcessorsRegisteredThenOtherProcessorRemovedTest, NodeTest::TestFixture) {
        // Register the processors
        node->addProcessor(&mockOtherProcessor.get());
        fakeit::Verify(Method(mockOtherProcessor, getType)).Once();
        verifyAllMocksChecked();
        node->addProcessor(&mockSomeProcessor.get());
        fakeit::Verify(Method(mockSomeProcessor, getType)).Once();
        verifyAllMocksChecked();

        // With both processors registered, both messages are processed.
        node->messageReceived(&mockSomeMessagePacket.get());
        fakeit::Verify(Method(mockSomeMessagePacket, getMessage)).Once();
        fakeit::Verify(Method(mockSomeMessage, getType)).Once();
        fakeit::Verify(Method(mockSomeProcessor, process).Using(&mockSomeMessage.get())).Once();
        node->messageReceived(&mockOtherMessagePacket.get());
        fakeit::Verify(Method(mockOtherMessagePacket, getMessage)).Once();
        fakeit::Verify(Method(mockOtherMessage, getType)).Once();
        fakeit::Verify(Method(mockOtherProcessor, process).Using(&mockOtherMessage.get())).Once();
        verifyAllMocksChecked();

        // Remove a processor
        node->removeProcessor(&mockOtherProcessor.get());
        fakeit::Verify(Method(mockOtherProcessor, getType)).Exactly(2);
        verifyAllMocksChecked();

        // Resend the messages, but only one is processed
        node->messageReceived(&mockSomeMessagePacket.get());
        fakeit::Verify(Method(mockSomeMessagePacket, getMessage)).Exactly(2);
        fakeit::Verify(Method(mockSomeMessage, getType)).Exactly(2);
        fakeit::Verify(Method(mockSomeProcessor, process).Using(&mockSomeMessage.get())).Exactly(2);
        node->messageReceived(&mockOtherMessagePacket.get());
        fakeit::Verify(Method(mockOtherMessagePacket, getMessage)).Exactly(2);
        fakeit::Verify(Method(mockOtherMessage, getType)).Exactly(2);
        verifyAllMocksChecked();

        // Remove the other processor
        node->removeProcessor(&mockSomeProcessor.get());
        fakeit::Verify(Method(mockSomeProcessor, getType)).Exactly(2);
        verifyAllMocksChecked();

        // Resend the messages, none are processed
        node->messageReceived(&mockSomeMessagePacket.get());
        fakeit::Verify(Method(mockSomeMessagePacket, getMessage)).Exactly(3);
        fakeit::Verify(Method(mockSomeMessage, getType)).Exactly(3);
        node->messageReceived(&mockOtherMessagePacket.get());
        fakeit::Verify(Method(mockOtherMessagePacket, getMessage)).Exactly(3);
        fakeit::Verify(Method(mockOtherMessage, getType)).Exactly(3);
        verifyAllMocksChecked();
    }

    BOOST_AUTO_TEST_SUITE_END()
