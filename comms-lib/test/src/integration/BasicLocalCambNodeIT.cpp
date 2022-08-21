
#define BOOST_TEST_DYN_LINK
#ifdef STAND_ALONE
#   define BOOST_TEST_MODULE Main
#endif
#include <boost/test/unit_test.hpp>

#include "comms/node/Node.h"
#include "comms/connection/LocalConnection.h"
#include "comms/connection/ConnectionException.h"
#include "comms/bus/BasicBus.h"
#include "comms/network/serializer/local/Serializer.h"

#include "TestMessage.h"

// Helper classes for the LocalBasicBusTest
namespace BasicLocalNodeITTest {

    /**
     * Message processor to use for the test, it merely tracks what message was received and allow the test to verify
     * if the correct expected instance was received.
     */
    template<class T>
    struct TestMessageProcessor: cadf::comms::MessageProcessor<T> {

            /**
             * Store what was received
             */
            virtual void processMessage(const T *msg) {
                receivedMsg = msg;
            }

            /**
             * Verify that the received message matches expectations. The last received message is cleared after is has been verified.
             *
             * @param *expected T pointer to the message that was expected to be received
             */
            virtual void verifyMessageReceived(T *expected) {
                BOOST_CHECK_EQUAL(expected, receivedMsg);
                receivedMsg = NULL;
            }

            /** The pointer to the message that was last received. */
            const T *receivedMsg = NULL;
    };

    /*
     * Helper fixture for initializing and preparing all of the necessary mocks
     */
    struct TestFixture {

            TestFixture() : bus(), msgRegistry(), msgFactory(), node1_1Connection(1, 1, &msgFactory), node1_2Connection(1, 2, &msgFactory), node2_1Connection(2, 1, &msgFactory),
                    node2_2Connection(2, 2, &msgFactory) {
                BOOST_CHECK_EQUAL(1, node1_1Connection.getType());
                BOOST_CHECK_EQUAL(1, node1_1Connection.getInstance());
                BOOST_CHECK_EQUAL(1, node1_2Connection.getType());
                BOOST_CHECK_EQUAL(2, node1_2Connection.getInstance());
                BOOST_CHECK_EQUAL(2, node2_1Connection.getType());
                BOOST_CHECK_EQUAL(1, node1_1Connection.getInstance());
                BOOST_CHECK_EQUAL(2, node2_2Connection.getType());
                BOOST_CHECK_EQUAL(2, node1_2Connection.getInstance());

                msgRegistry.registerMessages(&msgFactory);

                node1_1Connection.registerBus(&bus);
                node1_2Connection.registerBus(&bus);
                node2_1Connection.registerBus(&bus);
                node2_2Connection.registerBus(&bus);

                node1_1 = new cadf::comms::Node(&node1_1Connection);
                node1_2 = new cadf::comms::Node(&node1_2Connection);
                node2_1 = new cadf::comms::Node(&node2_1Connection);
                node2_2 = new cadf::comms::Node(&node2_2Connection);
            }

            void connectNodes() {
                BOOST_CHECK(node1_1->connect());
                BOOST_CHECK(node1_2->connect());
                BOOST_CHECK(node2_1->connect());
                BOOST_CHECK(node2_2->connect());
            }

            void disconnectNodes() {
                BOOST_CHECK(node1_1->disconnect());
                BOOST_CHECK(node1_2->disconnect());
                BOOST_CHECK(node2_1->disconnect());
                BOOST_CHECK(node2_2->disconnect());
            }

            ~TestFixture() {
                delete (node1_1);
                delete (node1_2);
                delete (node2_1);
                delete (node2_2);
            }

            /**
             * Helper to verify what all messages have been received.
             *
             * @param *n1_1tm TestMessage1 pointer to the message that node1_1 is expected to have received
             * @param *n1_1tm2 TestMessage2 pointer to the message that node1_1 is expected to have received
             * @param *n1_2tm TestMessage1 pointer to the message that node1_2 is expected to have received
             * @param *n1_2tm2 TestMessage2 pointer to the message that node1_2 is expected to have received
             * @param *n2_1tm TestMessage1 pointer to the message that node2_1 is expected to have received
             * @param *n2_1tm2 TestMessage2 pointer to the message that node2_1 is expected to have received
             * @param *n2_2tm TestMessage1 pointer to the message that node2_2 is expected to have received
             * @param *n2_2tm2 TestMessage2 pointer to the message that node2_2 is expected to have received
             */
            void verifyMessagesReceived(TestMessage1 *n1_1tm, TestMessage2 *n1_1tm2, TestMessage1 *n1_2tm, TestMessage2 *n1_2tm2, TestMessage1 *n2_1tm, TestMessage2 *n2_1tm2, TestMessage1 *n2_2tm,
                    TestMessage2 *n2_2tm2) {
                node1_1TestMessageProcessor.verifyMessageReceived(n1_1tm);
                node1_1TestMessage2Processor.verifyMessageReceived(n1_1tm2);
                node1_2TestMessageProcessor.verifyMessageReceived(n1_2tm);
                node1_2TestMessage2Processor.verifyMessageReceived(n1_2tm2);
                node2_1TestMessageProcessor.verifyMessageReceived(n2_1tm);
                node2_1TestMessage2Processor.verifyMessageReceived(n2_1tm2);
                node2_2TestMessageProcessor.verifyMessageReceived(n2_2tm);
                node2_2TestMessage2Processor.verifyMessageReceived(n2_2tm2);
            }

            cadf::comms::Node *node1_1;
            cadf::comms::Node *node1_2;
            cadf::comms::Node *node2_1;
            cadf::comms::Node *node2_2;

            cadf::comms::BasicBus bus;

            cadf::comms::MessageRegistry<cadf::comms::local::LocalProtocol, TestMessage1, TestMessage2> msgRegistry;
            cadf::comms::MessageFactory<cadf::comms::local::LocalProtocol> msgFactory;

            cadf::comms::LocalConnection node1_1Connection;
            cadf::comms::LocalConnection node1_2Connection;
            cadf::comms::LocalConnection node2_1Connection;
            cadf::comms::LocalConnection node2_2Connection;

            TestMessageProcessor<TestMessage1> node1_1TestMessageProcessor;
            TestMessageProcessor<TestMessage2> node1_1TestMessage2Processor;
            TestMessageProcessor<TestMessage1> node1_2TestMessageProcessor;
            TestMessageProcessor<TestMessage2> node1_2TestMessage2Processor;
            TestMessageProcessor<TestMessage1> node2_1TestMessageProcessor;
            TestMessageProcessor<TestMessage2> node2_1TestMessage2Processor;
            TestMessageProcessor<TestMessage1> node2_2TestMessageProcessor;
            TestMessageProcessor<TestMessage2> node2_2TestMessage2Processor;
            TestMessageProcessor<TestMessage1> unusedTestMessageProcessor1;
            TestMessageProcessor<TestMessage1> unusedTestMessageProcessor2;
            TestMessageProcessor<TestMessage2> unusedTestMessage2Processor1;
            TestMessageProcessor<TestMessage2> unusedTestMessage2Processor2;

            TestMessage1 tm1_1;
            TestMessage1 tm1_2;
            TestMessage1 tm1_3;
            TestMessage1 tm1_4;
            TestMessage1 tm1_5;
            TestMessage1 tm1_6;
            TestMessage1 tm1_7;
            TestMessage1 tm1_8;

            TestMessage2 tm2_1;
            TestMessage2 tm2_2;
            TestMessage2 tm2_3;
            TestMessage2 tm2_4;
            TestMessage2 tm2_5;
            TestMessage2 tm2_6;
            TestMessage2 tm2_7;
            TestMessage2 tm2_8;

            TestMessage3 tm3_1;
            TestMessage3 tm3_2;
            TestMessage3 tm3_3;
            TestMessage3 tm3_4;
    };

}

BOOST_AUTO_TEST_SUITE(BasicLocalNodeIT_Test_Suite)

/**
 * Verify that no attempt is made to send a message when the node is disconnected
 */
    BOOST_FIXTURE_TEST_CASE(SendMessagesNotRegisteredWithFactoryDisconnectedTest, BasicLocalNodeITTest::TestFixture) {
        // Registered messages
        BOOST_CHECK(!node1_1->sendMessage(&tm1_1, cadf::comms::ConnectionConstants::BROADCAST, cadf::comms::ConnectionConstants::BROADCAST));
        BOOST_CHECK(!node1_1->sendMessage(&tm2_1, cadf::comms::ConnectionConstants::BROADCAST, cadf::comms::ConnectionConstants::BROADCAST));
        BOOST_CHECK(!node1_2->sendMessage(&tm1_2, cadf::comms::ConnectionConstants::BROADCAST, cadf::comms::ConnectionConstants::BROADCAST));
        BOOST_CHECK(!node1_2->sendMessage(&tm2_2, cadf::comms::ConnectionConstants::BROADCAST, cadf::comms::ConnectionConstants::BROADCAST));

        // Unregistered messages
        BOOST_CHECK(!node1_1->sendMessage(&tm3_1, cadf::comms::ConnectionConstants::BROADCAST, cadf::comms::ConnectionConstants::BROADCAST));
        BOOST_CHECK(!node1_1->sendMessage(&tm3_2, cadf::comms::ConnectionConstants::BROADCAST, cadf::comms::ConnectionConstants::BROADCAST));
        BOOST_CHECK(!node1_2->sendMessage(&tm3_3, cadf::comms::ConnectionConstants::BROADCAST, cadf::comms::ConnectionConstants::BROADCAST));
        BOOST_CHECK(!node1_2->sendMessage(&tm3_4, cadf::comms::ConnectionConstants::BROADCAST, cadf::comms::ConnectionConstants::BROADCAST));
    }

    /**
     * Verify that an exception is thrown when trying to send a message that wasn't registered with the message factory
     */
    BOOST_FIXTURE_TEST_CASE(SendMessagesNotRegisteredWithFactoryConnectedTest, BasicLocalNodeITTest::TestFixture) {
        connectNodes();
        BOOST_REQUIRE_THROW(node1_1->sendMessage(&tm3_1, cadf::comms::ConnectionConstants::BROADCAST, cadf::comms::ConnectionConstants::BROADCAST), cadf::comms::MessageSendingException);
        BOOST_REQUIRE_THROW(node1_1->sendMessage(&tm3_2, cadf::comms::ConnectionConstants::BROADCAST, cadf::comms::ConnectionConstants::BROADCAST), cadf::comms::MessageSendingException);
        BOOST_REQUIRE_THROW(node1_2->sendMessage(&tm3_3, cadf::comms::ConnectionConstants::BROADCAST, cadf::comms::ConnectionConstants::BROADCAST), cadf::comms::MessageSendingException);
        BOOST_REQUIRE_THROW(node1_2->sendMessage(&tm3_4, cadf::comms::ConnectionConstants::BROADCAST, cadf::comms::ConnectionConstants::BROADCAST), cadf::comms::MessageSendingException);
    }

    /**
     * Verify that trying to send messages when no processor is registered will fail
     */
    BOOST_FIXTURE_TEST_CASE(SendMessagesNoProcessorsAddedTest, BasicLocalNodeITTest::TestFixture) {
        connectNodes();
        // Try sending from node 1 to node 2
        BOOST_CHECK(node1_1->sendMessage(&tm1_1, cadf::comms::ConnectionConstants::BROADCAST, cadf::comms::ConnectionConstants::BROADCAST));
        BOOST_CHECK(node1_1->sendMessage(&tm2_1, cadf::comms::ConnectionConstants::BROADCAST, cadf::comms::ConnectionConstants::BROADCAST));
        // Try sending from node 2 to node 1
        BOOST_CHECK(node1_2->sendMessage(&tm1_2, cadf::comms::ConnectionConstants::BROADCAST, cadf::comms::ConnectionConstants::BROADCAST));
        BOOST_CHECK(node1_2->sendMessage(&tm2_2, cadf::comms::ConnectionConstants::BROADCAST, cadf::comms::ConnectionConstants::BROADCAST));

        // Noone received anything
        verifyMessagesReceived(NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
    }

    /**
     * Verify that when a processor is registered that message is delivered
     */
    BOOST_FIXTURE_TEST_CASE(SendMessagesOneProcessorAddedTest, BasicLocalNodeITTest::TestFixture) {
        // Repeatedly connecting makes no difference
        connectNodes();
        connectNodes();
        connectNodes();
        connectNodes();
        connectNodes();

        // Register the processor on node 2
        node1_2->addProcessor(&node1_2TestMessageProcessor);

        // Try sending from node 1 to node 2
        BOOST_CHECK(node1_1->sendMessage(&tm1_1, cadf::comms::ConnectionConstants::BROADCAST, cadf::comms::ConnectionConstants::BROADCAST));
        BOOST_CHECK(node1_1->sendMessage(&tm2_1, cadf::comms::ConnectionConstants::BROADCAST, cadf::comms::ConnectionConstants::BROADCAST));
        // Try sending from node 2 to node 1
        BOOST_CHECK(node1_2->sendMessage(&tm1_2, cadf::comms::ConnectionConstants::BROADCAST, cadf::comms::ConnectionConstants::BROADCAST));
        BOOST_CHECK(node1_2->sendMessage(&tm2_2, cadf::comms::ConnectionConstants::BROADCAST, cadf::comms::ConnectionConstants::BROADCAST));

        // Verify that only tm1_1 made it through
        verifyMessagesReceived(NULL, NULL, &tm1_1, NULL, NULL, NULL, NULL, NULL);

        // Register the processor on node 1
        node1_1->addProcessor(&node1_1TestMessage2Processor);

        // Try sending from node 1 to node 2
        BOOST_CHECK(node1_1->sendMessage(&tm1_3, cadf::comms::ConnectionConstants::BROADCAST, cadf::comms::ConnectionConstants::BROADCAST));
        BOOST_CHECK(node1_1->sendMessage(&tm2_3, cadf::comms::ConnectionConstants::BROADCAST, cadf::comms::ConnectionConstants::BROADCAST));
        // Try sending from node 2 to node 1
        BOOST_CHECK(node1_2->sendMessage(&tm1_4, cadf::comms::ConnectionConstants::BROADCAST, cadf::comms::ConnectionConstants::BROADCAST));
        BOOST_CHECK(node1_2->sendMessage(&tm2_4, cadf::comms::ConnectionConstants::BROADCAST, cadf::comms::ConnectionConstants::BROADCAST));

        // Verify that only tm1_1 and tm2_4 made it through
        verifyMessagesReceived(NULL, &tm2_4, &tm1_3, NULL, NULL, NULL, NULL, NULL);
    }

    /**
     * Verify that when a processor is registered that message is delivered
     */
    BOOST_FIXTURE_TEST_CASE(SendMessagesManyProcessorsAddedBroadcastToAllTest, BasicLocalNodeITTest::TestFixture) {
        connectNodes();
        // Register all of the processors
        node1_1->addProcessor(&node1_1TestMessageProcessor);
        node1_1->addProcessor(&node1_1TestMessage2Processor);
        node1_2->addProcessor(&node1_2TestMessageProcessor);
        node1_2->addProcessor(&node1_2TestMessage2Processor);
        node2_1->addProcessor(&node2_1TestMessageProcessor);
        node2_1->addProcessor(&node2_1TestMessage2Processor);
        node2_2->addProcessor(&node2_2TestMessageProcessor);
        node2_2->addProcessor(&node2_2TestMessage2Processor);

        // Sending messages from node1_1
        BOOST_CHECK(node1_1->sendMessage(&tm1_1, cadf::comms::ConnectionConstants::BROADCAST, cadf::comms::ConnectionConstants::BROADCAST));
        verifyMessagesReceived(NULL, NULL, &tm1_1, NULL, &tm1_1, NULL, &tm1_1, NULL);
        BOOST_CHECK(node1_1->sendMessage(&tm2_1, cadf::comms::ConnectionConstants::BROADCAST, cadf::comms::ConnectionConstants::BROADCAST));
        verifyMessagesReceived(NULL, NULL, NULL, &tm2_1, NULL, &tm2_1, NULL, &tm2_1);

        // Sending messages from node1_2
        BOOST_CHECK(node1_2->sendMessage(&tm1_2, cadf::comms::ConnectionConstants::BROADCAST, cadf::comms::ConnectionConstants::BROADCAST));
        verifyMessagesReceived(&tm1_2, NULL, NULL, NULL, &tm1_2, NULL, &tm1_2, NULL);
        BOOST_CHECK(node1_2->sendMessage(&tm2_2, cadf::comms::ConnectionConstants::BROADCAST, cadf::comms::ConnectionConstants::BROADCAST));
        verifyMessagesReceived(NULL, &tm2_2, NULL, NULL, NULL, &tm2_2, NULL, &tm2_2);
    }

    /**
     * Verify that disconnecting prevents further messages from being sent
     */
    BOOST_FIXTURE_TEST_CASE(DisconnectingBlocksFurtherSendingTest, SendMessagesManyProcessorsAddedBroadcastToAllTest) {
        // Invoke the SendMessagesManyProcessorsAddedTest test
        SendMessagesManyProcessorsAddedBroadcastToAllTest::test_method();

        // Disconnect stops the sending of messages
        disconnectNodes();
        SendMessagesNotRegisteredWithFactoryDisconnectedTest().test_method();
        // Repeatedly disconnecting doesn't change behavior
        disconnectNodes();
        SendMessagesNotRegisteredWithFactoryDisconnectedTest().test_method();
        disconnectNodes();
        SendMessagesNotRegisteredWithFactoryDisconnectedTest().test_method();
        disconnectNodes();
        SendMessagesNotRegisteredWithFactoryDisconnectedTest().test_method();

        // Can reconnect to send messages again
        SendMessagesManyProcessorsAddedBroadcastToAllTest::test_method();
        // Can do so repeatedly
        SendMessagesManyProcessorsAddedBroadcastToAllTest::test_method();
        SendMessagesManyProcessorsAddedBroadcastToAllTest::test_method();
        SendMessagesManyProcessorsAddedBroadcastToAllTest::test_method();
    }

    /**
     * Verify that when a processor is registered that message is delivered
     */
    BOOST_FIXTURE_TEST_CASE(SendMessagesManyProcessorsAddedThenRemovedTest, SendMessagesManyProcessorsAddedBroadcastToAllTest) {
        // Invoke the SendMessagesManyProcessorsAddedTest test
        SendMessagesManyProcessorsAddedBroadcastToAllTest::test_method();

        // *****************************
        // Remove a processor from node1_2
        // *****************************
        node1_2->removeProcessor(&node1_2TestMessageProcessor);

        // Sending messages from node1_1
        BOOST_CHECK(node1_1->sendMessage(&tm1_3, cadf::comms::ConnectionConstants::BROADCAST, cadf::comms::ConnectionConstants::BROADCAST));
        verifyMessagesReceived(NULL, NULL, NULL, NULL, &tm1_3, NULL, &tm1_3, NULL);
        BOOST_CHECK(node1_1->sendMessage(&tm2_3, cadf::comms::ConnectionConstants::BROADCAST, cadf::comms::ConnectionConstants::BROADCAST));
        verifyMessagesReceived(NULL, NULL, NULL, &tm2_3, NULL, &tm2_3, NULL, &tm2_3);

        // *****************************
        // Remove a processor from node2_1
        // *****************************
        node2_1->removeProcessor(&node2_1TestMessage2Processor);
        // Sending messages from node1_1
        BOOST_CHECK(node1_1->sendMessage(&tm1_4, cadf::comms::ConnectionConstants::BROADCAST, cadf::comms::ConnectionConstants::BROADCAST));
        verifyMessagesReceived(NULL, NULL, NULL, NULL, &tm1_4, NULL, &tm1_4, NULL);
        BOOST_CHECK(node1_1->sendMessage(&tm2_4, cadf::comms::ConnectionConstants::BROADCAST, cadf::comms::ConnectionConstants::BROADCAST));
        verifyMessagesReceived(NULL, NULL, NULL, &tm2_4, NULL, NULL, NULL, &tm2_4);

        // *****************************
        // Remove all remaining processors
        // *****************************
        node1_1->removeProcessor(&node1_1TestMessageProcessor);
        node1_1->removeProcessor(&node1_1TestMessage2Processor);
        node1_2->removeProcessor(&node1_2TestMessage2Processor);
        node2_1->removeProcessor(&node2_1TestMessageProcessor);
        node2_2->removeProcessor(&node2_2TestMessageProcessor);
        node2_2->removeProcessor(&node2_2TestMessage2Processor);

        // Send a message from each node
        BOOST_CHECK(node1_1->sendMessage(&tm1_5, cadf::comms::ConnectionConstants::BROADCAST, cadf::comms::ConnectionConstants::BROADCAST));
        verifyMessagesReceived(NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
        BOOST_CHECK(node1_1->sendMessage(&tm2_5, cadf::comms::ConnectionConstants::BROADCAST, cadf::comms::ConnectionConstants::BROADCAST));
        verifyMessagesReceived(NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
        BOOST_CHECK(node1_2->sendMessage(&tm1_6, cadf::comms::ConnectionConstants::BROADCAST, cadf::comms::ConnectionConstants::BROADCAST));
        verifyMessagesReceived(NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
        BOOST_CHECK(node1_2->sendMessage(&tm2_6, cadf::comms::ConnectionConstants::BROADCAST, cadf::comms::ConnectionConstants::BROADCAST));
        verifyMessagesReceived(NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
        BOOST_CHECK(node2_1->sendMessage(&tm1_7, cadf::comms::ConnectionConstants::BROADCAST, cadf::comms::ConnectionConstants::BROADCAST));
        verifyMessagesReceived(NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
        BOOST_CHECK(node2_1->sendMessage(&tm2_7, cadf::comms::ConnectionConstants::BROADCAST, cadf::comms::ConnectionConstants::BROADCAST));
        verifyMessagesReceived(NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
        BOOST_CHECK(node2_2->sendMessage(&tm1_8, cadf::comms::ConnectionConstants::BROADCAST, cadf::comms::ConnectionConstants::BROADCAST));
        verifyMessagesReceived(NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
        BOOST_CHECK(node2_2->sendMessage(&tm2_8, cadf::comms::ConnectionConstants::BROADCAST, cadf::comms::ConnectionConstants::BROADCAST));
        verifyMessagesReceived(NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
    }

    /**
     * Verify that when a processor is registered that message is delivered
     */
    BOOST_FIXTURE_TEST_CASE(SendMessagesManyProcessorsAddedThenWrongOneRemovedTest, SendMessagesManyProcessorsAddedBroadcastToAllTest) {
        // Invoke the SendMessagesManyProcessorsAddedTest test
        SendMessagesManyProcessorsAddedBroadcastToAllTest::test_method();

        // Remove processors that are not registered
        node1_1->removeProcessor(&unusedTestMessageProcessor1);
        node1_1->removeProcessor(&unusedTestMessage2Processor1);
        node1_2->removeProcessor(&unusedTestMessageProcessor2);
        node1_2->removeProcessor(&unusedTestMessage2Processor2);

        // Messages can still be sent as per normal
        BOOST_CHECK(node1_1->sendMessage(&tm1_3, cadf::comms::ConnectionConstants::BROADCAST, cadf::comms::ConnectionConstants::BROADCAST));
        verifyMessagesReceived(NULL, NULL, &tm1_3, NULL, &tm1_3, NULL, &tm1_3, NULL);
        BOOST_CHECK(node1_1->sendMessage(&tm2_3, cadf::comms::ConnectionConstants::BROADCAST, cadf::comms::ConnectionConstants::BROADCAST));
        verifyMessagesReceived(NULL, NULL, NULL, &tm2_3, NULL, &tm2_3, NULL, &tm2_3);
        BOOST_CHECK(node1_2->sendMessage(&tm1_4, cadf::comms::ConnectionConstants::BROADCAST, cadf::comms::ConnectionConstants::BROADCAST));
        verifyMessagesReceived(&tm1_4, NULL, NULL, NULL, &tm1_4, NULL, &tm1_4, NULL);
        BOOST_CHECK(node1_2->sendMessage(&tm2_4, cadf::comms::ConnectionConstants::BROADCAST, cadf::comms::ConnectionConstants::BROADCAST));
        verifyMessagesReceived(NULL, &tm2_4, NULL, NULL, NULL, &tm2_4, NULL, &tm2_4);
        BOOST_CHECK(node2_1->sendMessage(&tm1_5, cadf::comms::ConnectionConstants::BROADCAST, cadf::comms::ConnectionConstants::BROADCAST));
        verifyMessagesReceived(&tm1_5, NULL, &tm1_5, NULL, NULL, NULL, &tm1_5, NULL);
        BOOST_CHECK(node2_1->sendMessage(&tm2_5, cadf::comms::ConnectionConstants::BROADCAST, cadf::comms::ConnectionConstants::BROADCAST));
        verifyMessagesReceived(NULL, &tm2_5, NULL, &tm2_5, NULL, NULL, NULL, &tm2_5);
        BOOST_CHECK(node2_2->sendMessage(&tm1_6, cadf::comms::ConnectionConstants::BROADCAST, cadf::comms::ConnectionConstants::BROADCAST));
        verifyMessagesReceived(&tm1_6, NULL, &tm1_6, NULL, &tm1_6, NULL, NULL, NULL);
        BOOST_CHECK(node2_2->sendMessage(&tm2_6, cadf::comms::ConnectionConstants::BROADCAST, cadf::comms::ConnectionConstants::BROADCAST));
        verifyMessagesReceived(NULL, &tm2_6, NULL, &tm2_6, NULL, &tm2_6, NULL, NULL);
    }

    /**
     * Verify that when directing to a specific node, the message is received and processed
     */
    BOOST_FIXTURE_TEST_CASE(SendMessagesManyProcessorsAddedSendToSingleNodeTest, SendMessagesManyProcessorsAddedBroadcastToAllTest) {
        // Invoke the SendMessagesManyProcessorsAddedTest test
        SendMessagesManyProcessorsAddedBroadcastToAllTest::test_method();

        // Cannot send to self
        BOOST_CHECK(node1_1->sendMessage(&tm1_3, 1, 1));
        verifyMessagesReceived(NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
        BOOST_CHECK(node1_1->sendMessage(&tm2_3, 1, 1));
        verifyMessagesReceived(NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

        // Can send to another
        BOOST_CHECK(node2_2->sendMessage(&tm1_4, 1, 2));
        verifyMessagesReceived(NULL, NULL, &tm1_4, NULL, NULL, NULL, NULL, NULL);
        BOOST_CHECK(node2_1->sendMessage(&tm2_4, 1, 2));
        verifyMessagesReceived(NULL, NULL, NULL, &tm2_4, NULL, NULL, NULL, NULL);
    }

    /**
     * Verify that can send/process a message to all instances of a type
     */
    BOOST_FIXTURE_TEST_CASE(SendMessagesManyProcessorsAddedBroadcastToAllInstancesTest, SendMessagesManyProcessorsAddedBroadcastToAllTest) {
        // Invoke the SendMessagesManyProcessorsAddedTest test
        SendMessagesManyProcessorsAddedBroadcastToAllTest::test_method();

        // Broadcasting to self will not deliver to the sending node
        BOOST_CHECK(node1_1->sendMessage(&tm1_3, 1, cadf::comms::ConnectionConstants::BROADCAST));
        verifyMessagesReceived(NULL, NULL, &tm1_3, NULL, NULL, NULL, NULL, NULL);
        BOOST_CHECK(node1_1->sendMessage(&tm2_3, 1, cadf::comms::ConnectionConstants::BROADCAST));
        verifyMessagesReceived(NULL, NULL, NULL, &tm2_3, NULL, NULL, NULL, NULL);

        // Can send to another
        BOOST_CHECK(node2_2->sendMessage(&tm1_4, 1, cadf::comms::ConnectionConstants::BROADCAST));
        verifyMessagesReceived(&tm1_4, NULL, &tm1_4, NULL, NULL, NULL, NULL, NULL);
        BOOST_CHECK(node2_1->sendMessage(&tm2_4, 1, cadf::comms::ConnectionConstants::BROADCAST));
        verifyMessagesReceived(NULL, &tm2_4, NULL, &tm2_4, NULL, NULL, NULL, NULL);
    }

    /**
     * Verify that can send/process a message to a specific instance across all types
     */
    BOOST_FIXTURE_TEST_CASE(SendMessagesManyProcessorsAddedBroadcastToSingleInstanceOnAllTypesTest, SendMessagesManyProcessorsAddedBroadcastToAllTest) {
        // Invoke the SendMessagesManyProcessorsAddedTest test
        SendMessagesManyProcessorsAddedBroadcastToAllTest::test_method();

        // Can send to self
        BOOST_CHECK(node1_1->sendMessage(&tm1_3, cadf::comms::ConnectionConstants::BROADCAST, 1));
        verifyMessagesReceived(NULL, NULL, NULL, NULL, &tm1_3, NULL, NULL, NULL);
        BOOST_CHECK(node1_1->sendMessage(&tm2_3, cadf::comms::ConnectionConstants::BROADCAST, 1));
        verifyMessagesReceived(NULL, NULL, NULL, NULL, NULL, &tm2_3, NULL, NULL);

        // Can send to another
        BOOST_CHECK(node2_2->sendMessage(&tm1_4, cadf::comms::ConnectionConstants::BROADCAST, 1));
        verifyMessagesReceived(&tm1_4, NULL, NULL, NULL, &tm1_4, NULL, NULL, NULL);
        BOOST_CHECK(node2_1->sendMessage(&tm2_4, cadf::comms::ConnectionConstants::BROADCAST, 2));
        verifyMessagesReceived(NULL, NULL, NULL, &tm2_4, NULL, NULL, NULL, &tm2_4);
    }

    BOOST_AUTO_TEST_SUITE_END()
