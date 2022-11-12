
#define BOOST_TEST_DYN_LINK
#ifdef STAND_ALONE
#   define BOOST_TEST_MODULE Main
#endif
#include <boost/test/unit_test.hpp>

#include "comms/node/BridgeNode.h"
#include "comms/node/Node.h"
#include "comms/connection/LocalConnection.h"
#include "comms/bus/BasicBus.h"
#include "comms/network/serializer/local/Serializer.h"

#include "TestMessage.h"

// Helper classes for the BridgeNodeIT
namespace BridgeNodeIT {

    const int BRIDGE_CONNECTION_TYPE = 3;
    const int BRIDGE_CONNECTION_1_INSTANCE = 1;
    const int BRIDGE_CONNECTION_2_INSTANCE = 2;

    /**
     * MessageProcessor that verifies the instance of the message received
     */
    template<class T>
    class MessageReceiptVerifier: public cadf::comms::MessageProcessor<T> {
        public:
            /**
             * Store the message instance that was received
             */
            virtual void processMessage(const T *msg) {
                receivedMsg = msg;
            }

            /**
             * Verify that the received message matches the expected instance. Resets the stored instance after checking it.
             */
            void verifyReceivedMessage(T *expected) {
                BOOST_CHECK_EQUAL(expected, receivedMsg);
                receivedMsg = NULL;
            }

            /** The message that was last received */
            const T *receivedMsg = NULL;
    };

    /**
     * Helper for creating Message factories.
     */
    struct MessageFactoryCreator {
            /**
             * CTOR - registers the messages with the factory
             */
            MessageFactoryCreator(): msgFactory(512) {
                msgRegistry.registerMessages(&msgFactory);
            }

            /**
             * DTOR
             */
            ~MessageFactoryCreator() = default;

            cadf::comms::MessageRegistry<cadf::comms::local::LocalProtocol, TestMessage1, TestMessage2> msgRegistry;
            cadf::comms::MessageFactory<cadf::comms::local::LocalProtocol> msgFactory;
    };

    /**
     * Helper for creating a bus with two nodes
     */
    struct BusCreator {
            /**
             * CTOR - creates the bus and all of the nodes, associates the bus to the connection, and add the verification message processor.
             */
            BusCreator(int type) : node1Connection(type, 1, &BusCreator::factoryCreator.msgFactory), node2Connection(type, 2, &BusCreator::factoryCreator.msgFactory) {
                node1Connection.registerBus(&bus);
                node2Connection.registerBus(&bus);

                node1 = new cadf::comms::Node(&node1Connection);
                node2 = new cadf::comms::Node(&node2Connection);

                node1->addProcessor(&node1TestMessage1Verifier);
                node1->addProcessor(&node1TestMessage2Verifier);
                node2->addProcessor(&node2TestMessage1Verifier);
                node2->addProcessor(&node2TestMessage2Verifier);

                node1->connect();
                node2->connect();
            }

            /**
             * DTOR
             */
            ~BusCreator() {
                delete (node1);
                delete (node2);
            }

            /**
             * Verify that the message processors received the expected message instances.
             *
             * @param *n1m1 TestMessage1 pointer to the message Node1 was expected to receive
             * @param *n1m2 TestMessage2 pointer to the message Node1 was expected to receive
             * @param *n2m1 TestMessage1 pointer to the message Node2 was expected to receive
             * @param *n2m2 TestMessage2 pointer to the message Node2 was expected to receive
             */
            void verifyReceivedMessage(TestMessage1 *n1m1, TestMessage2 *n1m2, TestMessage1 *n2m1, TestMessage2 *n2m2) {
                node1TestMessage1Verifier.verifyReceivedMessage(n1m1);
                node1TestMessage2Verifier.verifyReceivedMessage(n1m2);
                node2TestMessage1Verifier.verifyReceivedMessage(n2m1);
                node2TestMessage2Verifier.verifyReceivedMessage(n2m2);
            }

            /** The factory creator, static so that all can make use of it */
            static MessageFactoryCreator factoryCreator;

            /** The bus that the nodes will communicate across */
            cadf::comms::BasicBus bus;
            /** The connection for node1 to the bus */
            cadf::comms::LocalConnection node1Connection;
            /** The connection for node2 to the bus */
            cadf::comms::LocalConnection node2Connection;

            cadf::comms::Node *node1;
            cadf::comms::Node *node2;

            /** Message verifiers for the nodes to check that they received the correct message instance */
            MessageReceiptVerifier<TestMessage1> node1TestMessage1Verifier;
            MessageReceiptVerifier<TestMessage2> node1TestMessage2Verifier;
            MessageReceiptVerifier<TestMessage1> node2TestMessage1Verifier;
            MessageReceiptVerifier<TestMessage2> node2TestMessage2Verifier;
    };

    // Initialize the static message factory creator
    MessageFactoryCreator BusCreator::factoryCreator = MessageFactoryCreator();

    /**
     * Test fixture for performing the common setup and configuration for performing a test.
     *
     * On the bridge:
     *
     * - internalConnection = bus1
     * ----- Connection Type = BRIDGE_CONNECTION_TYPE
     * ----- Connection Instance =  BRIDGE_CONNECTION_1_INSTANCE
     *
     * - externalConnection = bus2
     * ----- Connection Type = BRIDGE_CONNECTION_TYPE
     * ----- Connection Instance =  BRIDGE_CONNECTION_2_INSTANCE
     *
     * Additionally:
     *
     * - bus1.node1 = Type 1, Instance 1
     * - bus1.node2 = Type 1, Instance 2
     * - bus2.node1 = Type 2, Instance 1
     * - bus2.node2 = Type 2, Instance 2
     *
     * The overall layout can be drawn out as:
     *
     *   -----------      -----        ----------------------        -----      -----------
     *   | Node 1  |      |   |        |       BRIDGE       |        |   |      | Node 1  |
     *   | T=1,I=1 |<---->| B |        |                    |        | B |<---->| T=2,I=1 |
     *   -----------      | U |      ------------  ------------      | U |      -----------
     *                    | S |<---->| Internal |  | External |<---->| S |
     *   -----------      |   |      | T=3, I=1 |  | T=3, I=2 |      |   |      -----------
     *   | Node 2  |<---->| 1 |      ------------  ------------      | 2 |<---->| Node 2  |
     *   | T=1,I=2 |      |   |        |                    |        |   |      | T=2,I=2 |
     *   -----------      -----        ----------------------        -----      -----------
     *
     */
    struct TestFixture {
            /**
             * CTOR - assembles all of the nodes and bridges them.
             */
            TestFixture() : bus1(1), bus2(2), bus1Connection(BRIDGE_CONNECTION_TYPE, BRIDGE_CONNECTION_1_INSTANCE, &BusCreator::factoryCreator.msgFactory),
                    bus2Connection(BRIDGE_CONNECTION_TYPE, BRIDGE_CONNECTION_2_INSTANCE, &BusCreator::factoryCreator.msgFactory) {
                bus1Connection.registerBus(&bus1.bus);
                bus2Connection.registerBus(&bus2.bus);

                bridge = new cadf::comms::BridgeNode(&bus1Connection, &bus2Connection);
            }

            /**
             * DTOR
             */
            ~TestFixture() {
                delete (bridge);
            }

            /**
             * Verify that all nodes received the expected messages.
             *
             * @param *b1n1m1 TestMessage1 pointer to the message Bus 1 Node1 was expected to receive
             * @param *b1n1m2 TestMessage2 pointer to the message Bus 1 Node1 was expected to receive
             * @param *b1n2m1 TestMessage1 pointer to the message Bus 1 Node2 was expected to receive
             * @param *b1n2m2 TestMessage2 pointer to the message Bus 1 Node2 was expected to receive
             * @param *b2n1m1 TestMessage1 pointer to the message Bus 2 Node1 was expected to receive
             * @param *b2n1m2 TestMessage2 pointer to the message Bus 2 Node1 was expected to receive
             * @param *b2n2m1 TestMessage1 pointer to the message Bus 2 Node2 was expected to receive
             * @param *b2n2m2 TestMessage2 pointer to the message Bus 2 Node2 was expected to receive
             */
            void verifyReceivedMessage(TestMessage1 *b1n1m1, TestMessage2 *b1n1m2, TestMessage1 *b1n2m1, TestMessage2 *b1n2m2, TestMessage1 *b2n1m1, TestMessage2 *b2n1m2, TestMessage1 *b2n2m1,
                    TestMessage2 *b2n2m2) {
                bus1.verifyReceivedMessage(b1n1m1, b1n1m2, b1n2m1, b1n2m2);
                bus2.verifyReceivedMessage(b2n1m1, b2n1m2, b2n2m1, b2n2m2);
            }

            /** Creator which assembles Bus1 and its connected nodes */
            BusCreator bus1;
            /** Creator which assembles Bus2 and its connected nodes */
            BusCreator bus2;
            /** The Internal connection from the Bridge to Bus1 */
            cadf::comms::LocalConnection bus1Connection;
            /** The External connection from the Bridge to Bus2 */
            cadf::comms::LocalConnection bus2Connection;
            /** The bridge that is to be tested */
            cadf::comms::BridgeNode *bridge;

            /** Messages to be used for testing */
            TestMessage1 msg1;
            TestMessage2 msg2;
    };
}

BOOST_AUTO_TEST_SUITE(BridgeNodeIT_Test_Suite)

/**
 * Verify that when no forwarding rules are creating, no messages are forwarded
 */
    BOOST_FIXTURE_TEST_CASE(SendMessageNoForwardingRulesTest, BridgeNodeIT::TestFixture) {
        // If Bus 1 Node 1 broadcasts a message, only Bus 1 Node 2 can receive it
        bus1.node1->sendMessage(&msg1, cadf::comms::ConnectionConstants::BROADCAST, cadf::comms::ConnectionConstants::BROADCAST);
        verifyReceivedMessage(NULL, NULL, &msg1, NULL, NULL, NULL, NULL, NULL);
        bus1.node1->sendMessage(&msg2, cadf::comms::ConnectionConstants::BROADCAST, cadf::comms::ConnectionConstants::BROADCAST);
        verifyReceivedMessage(NULL, NULL, NULL, &msg2, NULL, NULL, NULL, NULL);
        // If Bus 1 Node 2 sends any message, only Bus 1 Node 1 can receive it
        bus1.node2->sendMessage(&msg1, cadf::comms::ConnectionConstants::BROADCAST, cadf::comms::ConnectionConstants::BROADCAST);
        verifyReceivedMessage(&msg1, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
        bus1.node2->sendMessage(&msg2, cadf::comms::ConnectionConstants::BROADCAST, cadf::comms::ConnectionConstants::BROADCAST);
        verifyReceivedMessage(NULL, &msg2, NULL, NULL, NULL, NULL, NULL, NULL);

        // If Bus 2 Node 1 broadcasts a message, only Bus 1 Node 2 can receive it
        bus2.node1->sendMessage(&msg1, cadf::comms::ConnectionConstants::BROADCAST, cadf::comms::ConnectionConstants::BROADCAST);
        verifyReceivedMessage(NULL, NULL, NULL, NULL, NULL, NULL, &msg1, NULL);
        bus2.node1->sendMessage(&msg2, cadf::comms::ConnectionConstants::BROADCAST, cadf::comms::ConnectionConstants::BROADCAST);
        verifyReceivedMessage(NULL, NULL, NULL, NULL, NULL, NULL, NULL, &msg2);
        // If Bus 2 Node 2 sends any message, only Bus 1 Node 1 can receive it
        bus2.node2->sendMessage(&msg1, cadf::comms::ConnectionConstants::BROADCAST, cadf::comms::ConnectionConstants::BROADCAST);
        verifyReceivedMessage(NULL, NULL, NULL, NULL, &msg1, NULL, NULL, NULL);
        bus2.node2->sendMessage(&msg2, cadf::comms::ConnectionConstants::BROADCAST, cadf::comms::ConnectionConstants::BROADCAST);
        verifyReceivedMessage(NULL, NULL, NULL, NULL, NULL, &msg2, NULL, NULL);
    }

    /**
     * Verify that when forwarding rules from bus1 to bus2 are created, messages can be forwarded in that direction
     */
    BOOST_FIXTURE_TEST_CASE(SendMessageForwardFromOneToTwoTest, BridgeNodeIT::TestFixture) {
        bridge->addForwardToExternalRule("TestMessage1", 2, 1);

        // If the bridge receives a TestMessage1 from Bus 1, it will be forwarded to Bus 2 Node 1
        // A broadcast message will be sent
        bus1.node1->sendMessage(&msg1, cadf::comms::ConnectionConstants::BROADCAST, cadf::comms::ConnectionConstants::BROADCAST);
        verifyReceivedMessage(NULL, NULL, &msg1, NULL, &msg1, NULL, NULL, NULL);
        // Sending it to the bridge will forward it, but not be received internally within the bus
        bus1.node1->sendMessage(&msg1, BridgeNodeIT::BRIDGE_CONNECTION_TYPE, BridgeNodeIT::BRIDGE_CONNECTION_1_INSTANCE);
        verifyReceivedMessage(NULL, NULL, NULL, NULL, &msg1, NULL, NULL, NULL);
        // Sending it to Node 2 will not forward it
        bus1.node1->sendMessage(&msg1, 1, 2);
        // Message 2 is not being forwarded
        verifyReceivedMessage(NULL, NULL, &msg1, NULL, NULL, NULL, NULL, NULL);
        bus1.node1->sendMessage(&msg2, cadf::comms::ConnectionConstants::BROADCAST, cadf::comms::ConnectionConstants::BROADCAST);
        verifyReceivedMessage(NULL, NULL, NULL, &msg2, NULL, NULL, NULL, NULL);
        // If Bus 1 Node 2 sends TestMessage1, it will also be forwarded
        bus1.node2->sendMessage(&msg1, cadf::comms::ConnectionConstants::BROADCAST, cadf::comms::ConnectionConstants::BROADCAST);
        verifyReceivedMessage(&msg1, NULL, NULL, NULL, &msg1, NULL, NULL, NULL);
        bus1.node2->sendMessage(&msg2, cadf::comms::ConnectionConstants::BROADCAST, cadf::comms::ConnectionConstants::BROADCAST);
        verifyReceivedMessage(NULL, &msg2, NULL, NULL, NULL, NULL, NULL, NULL);

        // No forwarding rule from Bus2, no messages will be forwarded to Bus 1
        bus2.node1->sendMessage(&msg1, cadf::comms::ConnectionConstants::BROADCAST, cadf::comms::ConnectionConstants::BROADCAST);
        verifyReceivedMessage(NULL, NULL, NULL, NULL, NULL, NULL, &msg1, NULL);
        bus2.node1->sendMessage(&msg2, cadf::comms::ConnectionConstants::BROADCAST, cadf::comms::ConnectionConstants::BROADCAST);
        verifyReceivedMessage(NULL, NULL, NULL, NULL, NULL, NULL, NULL, &msg2);
        // No forwarding rule from Bus2, no messages will be forwarded to Bus 1
        bus2.node2->sendMessage(&msg1, cadf::comms::ConnectionConstants::BROADCAST, cadf::comms::ConnectionConstants::BROADCAST);
        verifyReceivedMessage(NULL, NULL, NULL, NULL, &msg1, NULL, NULL, NULL);
        bus2.node2->sendMessage(&msg2, cadf::comms::ConnectionConstants::BROADCAST, cadf::comms::ConnectionConstants::BROADCAST);
        verifyReceivedMessage(NULL, NULL, NULL, NULL, NULL, &msg2, NULL, NULL);
    }

    /**
     * Verify that can broadcast message on forward in both directions
     */
    BOOST_FIXTURE_TEST_CASE(ForwardMultipleMessagesTest, BridgeNodeIT::TestFixture) {
        bridge->addForwardToInternalRule("TestMessage1", 1, 2);
        bridge->addForwardToInternalRule("TestMessage2", 1, 1);

        // If Bus 1 Node 1 broadcasts a message, only Bus 1 Node 2 can receive it
        bus1.node1->sendMessage(&msg1, cadf::comms::ConnectionConstants::BROADCAST, cadf::comms::ConnectionConstants::BROADCAST);
        verifyReceivedMessage(NULL, NULL, &msg1, NULL, NULL, NULL, NULL, NULL);
        bus1.node1->sendMessage(&msg2, cadf::comms::ConnectionConstants::BROADCAST, cadf::comms::ConnectionConstants::BROADCAST);
        verifyReceivedMessage(NULL, NULL, NULL, &msg2, NULL, NULL, NULL, NULL);
        // If Bus 1 Node 2 sends any message, only Bus 1 Node 1 can receive it
        bus1.node2->sendMessage(&msg1, cadf::comms::ConnectionConstants::BROADCAST, cadf::comms::ConnectionConstants::BROADCAST);
        verifyReceivedMessage(&msg1, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
        bus1.node2->sendMessage(&msg2, cadf::comms::ConnectionConstants::BROADCAST, cadf::comms::ConnectionConstants::BROADCAST);
        verifyReceivedMessage(NULL, &msg2, NULL, NULL, NULL, NULL, NULL, NULL);

        //Message sent from Bus2 will be forwarded to Bus1
        bus2.node1->sendMessage(&msg1, cadf::comms::ConnectionConstants::BROADCAST, cadf::comms::ConnectionConstants::BROADCAST);
        verifyReceivedMessage(NULL, NULL, &msg1, NULL, NULL, NULL, &msg1, NULL);
        bus2.node1->sendMessage(&msg2, BridgeNodeIT::BRIDGE_CONNECTION_TYPE, BridgeNodeIT::BRIDGE_CONNECTION_2_INSTANCE);
        verifyReceivedMessage(NULL, &msg2, NULL, NULL, NULL, NULL, NULL, NULL);
        bus2.node2->sendMessage(&msg1, cadf::comms::ConnectionConstants::BROADCAST, BridgeNodeIT::BRIDGE_CONNECTION_2_INSTANCE);
        verifyReceivedMessage(NULL, NULL, &msg1, NULL, NULL, NULL, NULL, NULL);
        // If address to self, no one receives anything
        bus2.node2->sendMessage(&msg1, 2, BridgeNodeIT::BRIDGE_CONNECTION_2_INSTANCE);
        verifyReceivedMessage(NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
    }

    /**
     * Verify that can forward TestMessage2 in both directions
     */
    BOOST_FIXTURE_TEST_CASE(ForwardMessagesBackAndForthTest, BridgeNodeIT::TestFixture) {
        bridge->addForwardToInternalRule("TestMessage2", 1, 1);
        bridge->addForwardToExternalRule("TestMessage2", 2, 2);

        // Can forward messages from bus1 to bus2
        bus1.node2->sendMessage(&msg2, BridgeNodeIT::BRIDGE_CONNECTION_TYPE, BridgeNodeIT::BRIDGE_CONNECTION_1_INSTANCE);
        verifyReceivedMessage(NULL, NULL, NULL, NULL, NULL, NULL, NULL, &msg2);
        // Can broadcast message from bus2 to bus1
        bus2.node1->sendMessage(&msg2, cadf::comms::ConnectionConstants::BROADCAST, cadf::comms::ConnectionConstants::BROADCAST);
        verifyReceivedMessage(NULL, &msg2, NULL, NULL, NULL, NULL, NULL, &msg2);
    }

    /**
     * Verify that can broadcast message on forward in both directions
     */
    BOOST_FIXTURE_TEST_CASE(BroadcastForwardedMessageTest, BridgeNodeIT::TestFixture) {
        bridge->addForwardToInternalRule("TestMessage1", cadf::comms::ConnectionConstants::BROADCAST, cadf::comms::ConnectionConstants::BROADCAST);
        bridge->addForwardToInternalRule("TestMessage2", cadf::comms::ConnectionConstants::BROADCAST, cadf::comms::ConnectionConstants::BROADCAST);
        bridge->addForwardToExternalRule("TestMessage1", cadf::comms::ConnectionConstants::BROADCAST, cadf::comms::ConnectionConstants::BROADCAST);
        bridge->addForwardToExternalRule("TestMessage2", cadf::comms::ConnectionConstants::BROADCAST, cadf::comms::ConnectionConstants::BROADCAST);

        // Message sent from Bus1 will be received by all
        bus1.node2->sendMessage(&msg2, cadf::comms::ConnectionConstants::BROADCAST, cadf::comms::ConnectionConstants::BROADCAST);
        verifyReceivedMessage(NULL, &msg2, NULL, NULL, NULL, &msg2, NULL, &msg2);
        bus1.node1->sendMessage(&msg1, cadf::comms::ConnectionConstants::BROADCAST, cadf::comms::ConnectionConstants::BROADCAST);
        verifyReceivedMessage(NULL, NULL, &msg1, NULL, &msg1, NULL, &msg1, NULL);

        // Message sent from Bus2 wull be received by all
        bus2.node1->sendMessage(&msg1, cadf::comms::ConnectionConstants::BROADCAST, cadf::comms::ConnectionConstants::BROADCAST);
        verifyReceivedMessage(&msg1, NULL, &msg1, NULL, NULL, NULL, &msg1, NULL);
        bus2.node2->sendMessage(&msg2, cadf::comms::ConnectionConstants::BROADCAST, cadf::comms::ConnectionConstants::BROADCAST);
        verifyReceivedMessage(NULL, &msg2, NULL, &msg2, NULL, &msg2, NULL, NULL);
    }

    BOOST_AUTO_TEST_SUITE_END()
