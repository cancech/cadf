
#define BOOST_TEST_DYN_LINK
#ifdef STAND_ALONE
#   define BOOST_TEST_MODULE Main
#endif
#include <boost/test/unit_test.hpp>

#include "comms/network/serializer/binary/Serializer.h"
#include "comms/network/serializer/dom/Serializer.h"

#include "TestServer.h"
#include "TestNetNode.h"

namespace ClientConnectionIT {

    /**
     * Build the server info required for the connection
     *
     * @param port int
     */
    cadf::comms::NetworkInfo serverInfo(int port) {
        cadf::comms::NetworkInfo serverNetInfo;
        serverNetInfo.ipVersion = cadf::comms::NetworkInfo::IPv4;
        serverNetInfo.netAddress = "127.0.0.1";
        serverNetInfo.port = port;
        return serverNetInfo;
    }

    /**
     * Helper to pause the test for a moment to give the thread being tested a moment in which to progress its execution.
     */
    void giveThreadSomeTime() {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    /**
     * Perform the test of initializing and connecting all nodes and the bus, and perform the steps required to make sure
     * that messages can be passed back and forth between all parties.
     *
     * @param port int the port at which the server bus is to run.
     */
    template<class PROTOCOL>
    void performTest(int portNum) {
        cadf::comms::NetworkInfo netInfo = ClientConnectionIT::serverInfo(portNum);

        test::TestServer<PROTOCOL> server(netInfo);
        // Start the server
        BOOST_CHECK(!server.isUp());
        BOOST_CHECK(server.start());
        BOOST_CHECK(server.isUp());

        // Initialize and connect Client1
        test::TestMessage1Processor client1Processor;
        test::TestNetNode<PROTOCOL> client1(1, 1, netInfo);
        client1.addProcessor(&client1Processor);

        BOOST_CHECK(!client1.isConnected());
        BOOST_CHECK(client1.connect());
        BOOST_CHECK(client1.isConnected());


        // Initialize and connect Client2
        test::TestMessage1Processor client2Processor;
        test::TestNetNode<PROTOCOL> client2(2, 1, netInfo);
        client2.addProcessor(&client2Processor);

        BOOST_CHECK(!client2.isConnected());
        BOOST_CHECK(client2.connect());
        BOOST_CHECK(client2.isConnected());

        // Wait for handshaking to complete
        giveThreadSomeTime();

        // Send a test message from Client1 to Client2
        TestData data1 = {1, 1.23};
        TestMessage1 msg1(data1);
        BOOST_CHECK(client1.sendMessage(&msg1, 2, 1));
        giveThreadSomeTime();

        client1Processor.verifyState(0, -1, -2);
        client2Processor.verifyState(1, 1, 1.23);
        giveThreadSomeTime();

        // Send a test message from Client2 to Client1
        TestData data2 = {234, 4.56};
        TestMessage1 msg2(data2);
        BOOST_CHECK(client2.sendMessage(&msg2, 1, 1));
        giveThreadSomeTime();

        client1Processor.verifyState(1, 234, 4.56);
        client2Processor.verifyState(1, -1, -2);
        giveThreadSomeTime();

        // Stop the clients
        BOOST_CHECK(client1.disconnect());
        BOOST_CHECK(!client1.isConnected());
        BOOST_CHECK(client2.disconnect());
        BOOST_CHECK(!client2.isConnected());

        // Verify that can no longer send messages
        BOOST_CHECK(!client1.sendMessage(&msg1, 2, 1));
        BOOST_CHECK(!client2.sendMessage(&msg2, 1, 1));

        // Terminate the server
        BOOST_CHECK(server.isUp());
        BOOST_CHECK(server.stop());
        BOOST_CHECK(!server.isUp());
    }

}

/**
 * Test suite for validating the ability to send/receive Binary serialized messages across the network
 */
BOOST_AUTO_TEST_SUITE(ClientConnectionIT_Test_Suite)

/**
 * Verify that it is possible to send and receive messages when using the Binary protocol
 */
    BOOST_AUTO_TEST_CASE(BinaryConnectAndMessageTest) {
        ClientConnectionIT::performTest<cadf::comms::binary::BinaryProtocol>(4321);
    }

    /**
     * Verify that it is possible to send and receive messages when using the JSON protocol
     */
    BOOST_AUTO_TEST_CASE(JSONConnectAndMessageTest) {
        ClientConnectionIT::performTest<cadf::comms::dom::json::JSONProtocol>(1234);
    }

    BOOST_AUTO_TEST_SUITE_END()
