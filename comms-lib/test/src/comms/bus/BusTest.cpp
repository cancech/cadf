#define BOOST_TEST_DYN_LINK
#ifdef STAND_ALONE
#   define BOOST_TEST_MODULE Main
#endif
#include <boost/test/unit_test.hpp>
#include <fakeit.hpp>

#include "comms/bus/Bus.h"

// Helper classes for the LocalSimpleBusTest
namespace BusTest {

    /**
     * Concrete implementation of the AbstractBus to be used for testing.
     */
    class TestBus: public cadf::comms::AbstractBus {
        public:
            virtual ~TestBus() = default;

            virtual void sendMessage(cadf::comms::IBusConnection *sender, const cadf::comms::MessagePacket *packet) {
                routeMessage(sender, packet);
            }

            virtual void sendMessage(cadf::comms::IBusConnection *sender, cadf::comms::IBusConnection *recipient, const cadf::comms::MessagePacket *packet) {
                recipient->sendMessage(sender, packet);
            }
    };

    /*
     * Helper fixture for initializing and preparing all of the necessary mocks
     */
    struct SetupMocks {

            SetupMocks() {
                stubMockConnection(mockConn1_1_1, 1, 1);
                stubMockConnection(mockConn1_1_2, 1, 1);
                stubMockConnection(mockConn1_1_3, 1, 1);
                stubMockConnection(mockConn1_2, 1, 2);
                stubMockConnection(mockConn2_1, 2, 1);
                stubMockConnection(mockConn2_2, 2, 2);
                stubMockConnection(mockConn3_1, 3, 1);
                stubMockConnection(mockConn3_2, 3, 2);
            }

            void stubMockConnection(fakeit::Mock<cadf::comms::IBusConnection> &mockConnection, int type, int instance) {
                fakeit::When(Method(mockConnection, getType)).AlwaysReturn(type);
                fakeit::When(Method(mockConnection, getInstance)).AlwaysReturn(instance);
                fakeit::When(Method(mockConnection, sendMessage)).AlwaysReturn();
            }

            ~SetupMocks() {
                verifyAllMocksChecked();
            }

            void verifyAllMocksChecked() {
                fakeit::VerifyNoOtherInvocations(mockConn1_1_1, mockConn1_1_2, mockConn1_1_3, mockConn1_2, mockConn2_1, mockConn2_2, mockConn3_1, mockConn3_2, mockPacket1, mockPacket2, mockPacket3, mockPacket4);
            }

            fakeit::Mock<cadf::comms::IBusConnection> mockConn1_1_1;
            fakeit::Mock<cadf::comms::IBusConnection> mockConn1_1_2;
            fakeit::Mock<cadf::comms::IBusConnection> mockConn1_1_3;
            fakeit::Mock<cadf::comms::IBusConnection> mockConn1_2;
            fakeit::Mock<cadf::comms::IBusConnection> mockConn2_1;
            fakeit::Mock<cadf::comms::IBusConnection> mockConn2_2;
            fakeit::Mock<cadf::comms::IBusConnection> mockConn3_1;
            fakeit::Mock<cadf::comms::IBusConnection> mockConn3_2;
            fakeit::Mock<cadf::comms::MessagePacket> mockPacket1;
            fakeit::Mock<cadf::comms::MessagePacket> mockPacket2;
            fakeit::Mock<cadf::comms::MessagePacket> mockPacket3;
            fakeit::Mock<cadf::comms::MessagePacket> mockPacket4;
            fakeit::Mock<cadf::comms::MessagePacket> mockPacket5;
    };

    struct TestFixtureAllConnectionsConnected: public SetupMocks {

            TestBus bus;

            TestFixtureAllConnectionsConnected() {
                // Establish the connections
                connectedMockConnection(mockConn1_1_1);
                connectedMockConnection(mockConn1_1_2);
                connectedMockConnection(mockConn1_1_3);
                connectedMockConnection(mockConn1_2);
                connectedMockConnection(mockConn2_1);
                connectedMockConnection(mockConn2_2);
                connectedMockConnection(mockConn3_1);
                connectedMockConnection(mockConn3_2);
            }

            void connectedMockConnection(fakeit::Mock<cadf::comms::IBusConnection> &mockConnection) {
                bus.connected(&mockConnection.get());
                fakeit::Verify(Method(mockConnection, getType)).Once();
                fakeit::Verify(Method(mockConnection, getInstance)).Once();
                verifyAllMocksChecked();
            }
    };
}

/**
 * Unit test for the LocalSimpleBus
 */
BOOST_AUTO_TEST_SUITE(AbstractBus_Test_Suite)

/**
 * Verify that when no connection is present, sending the message does nothing
 */
    BOOST_FIXTURE_TEST_CASE(NoConnectionSendMessageTest, BusTest::SetupMocks) {
        BusTest::TestBus bus;
        verifyAllMocksChecked();

        // Since there are no connections, doesn't matter who the recipient is
        fakeit::When(Method(mockPacket1, isTypeBroadcast)).AlwaysReturn(true);
        fakeit::When(Method(mockPacket1, isInstanceBroadcast)).AlwaysReturn(true);
        fakeit::When(Method(mockPacket2, isTypeBroadcast)).AlwaysReturn(true);
        fakeit::When(Method(mockPacket2, isInstanceBroadcast)).AlwaysReturn(true);

        bus.sendMessage(&mockConn1_1_1.get(), &mockPacket1.get());
        fakeit::Verify(Method(mockPacket1, isTypeBroadcast)).Once();
        fakeit::Verify(Method(mockPacket1, isInstanceBroadcast)).Once();
        verifyAllMocksChecked();
        bus.sendMessage(&mockConn2_2.get(), &mockPacket2.get());
        fakeit::Verify(Method(mockPacket2, isTypeBroadcast)).Once();
        fakeit::Verify(Method(mockPacket2, isInstanceBroadcast)).Once();
        verifyAllMocksChecked();
    }

    /**
     * Verify that when no connection is present, sending the message does nothing
     */
    BOOST_FIXTURE_TEST_CASE(DuplicateConnectionAddressesMessageTest, BusTest::SetupMocks) {
        BusTest::TestBus bus;
        verifyAllMocksChecked();

        // Connect the three instances
        bus.connected(&mockConn1_1_1.get());
        fakeit::Verify(Method(mockConn1_1_1, getType)).Once();
        fakeit::Verify(Method(mockConn1_1_1, getInstance)).Once();
        bus.connected(&mockConn1_1_2.get());
        fakeit::Verify(Method(mockConn1_1_2, getType)).Once();
        fakeit::Verify(Method(mockConn1_1_2, getInstance)).Once();
        bus.connected(&mockConn1_1_3.get());
        fakeit::Verify(Method(mockConn1_1_3, getType)).Once();
        fakeit::Verify(Method(mockConn1_1_3, getInstance)).Once();
        bus.connected(&mockConn1_2.get());
        fakeit::Verify(Method(mockConn1_2, getType)).Once();
        fakeit::Verify(Method(mockConn1_2, getInstance)).Once();
        verifyAllMocksChecked();

        // Send a message to connections on the same type/instance, sender doesn't receive it but everyone else does
        fakeit::When(Method(mockPacket1, isTypeBroadcast)).AlwaysReturn(false);
        fakeit::When(Method(mockPacket1, getRecipientType)).AlwaysReturn(1);
        fakeit::When(Method(mockPacket1, isInstanceBroadcast)).AlwaysReturn(false);
        fakeit::When(Method(mockPacket1, getRecipientInstance)).AlwaysReturn(1);
        bus.sendMessage(&mockConn1_1_1.get(), &mockPacket1.get());
        fakeit::Verify(Method(mockPacket1, isTypeBroadcast)).Once();
        fakeit::Verify(Method(mockPacket1, getRecipientType)).Once();
        fakeit::Verify(Method(mockPacket1, isInstanceBroadcast)).Once();
        fakeit::Verify(Method(mockPacket1, getRecipientInstance)).Once();
        fakeit::Verify(Method(mockConn1_1_2, sendMessage).Using(&mockConn1_1_1.get(), &mockPacket1.get())).Once();
        fakeit::Verify(Method(mockConn1_1_3, sendMessage).Using(&mockConn1_1_1.get(), &mockPacket1.get())).Once();
        verifyAllMocksChecked();

        // Send from a different connection and all copies receive it
        fakeit::When(Method(mockPacket2, isTypeBroadcast)).AlwaysReturn(false);
        fakeit::When(Method(mockPacket2, getRecipientType)).AlwaysReturn(1);
        fakeit::When(Method(mockPacket2, isInstanceBroadcast)).AlwaysReturn(false);
        fakeit::When(Method(mockPacket2, getRecipientInstance)).AlwaysReturn(1);
        bus.sendMessage(&mockConn1_2.get(), &mockPacket2.get());
        fakeit::Verify(Method(mockPacket2, isTypeBroadcast)).Once();
        fakeit::Verify(Method(mockPacket2, getRecipientType)).Once();
        fakeit::Verify(Method(mockPacket2, isInstanceBroadcast)).Once();
        fakeit::Verify(Method(mockPacket2, getRecipientInstance)).Once();
        fakeit::Verify(Method(mockConn1_1_1, sendMessage).Using(&mockConn1_2.get(), &mockPacket2.get())).Once();
        fakeit::Verify(Method(mockConn1_1_2, sendMessage).Using(&mockConn1_2.get(), &mockPacket2.get())).Once();
        fakeit::Verify(Method(mockConn1_1_3, sendMessage).Using(&mockConn1_2.get(), &mockPacket2.get())).Once();
        verifyAllMocksChecked();

        // Remove one and make sure that it ceases to receive the messages
        bus.disconnected(&mockConn1_1_2.get());
        fakeit::Verify(Method(mockConn1_1_2, getType)).Exactly(2);
        fakeit::Verify(Method(mockConn1_1_2, getInstance)).Exactly(2);

        fakeit::When(Method(mockPacket3, isTypeBroadcast)).AlwaysReturn(false);
        fakeit::When(Method(mockPacket3, getRecipientType)).AlwaysReturn(1);
        fakeit::When(Method(mockPacket3, isInstanceBroadcast)).AlwaysReturn(false);
        fakeit::When(Method(mockPacket3, getRecipientInstance)).AlwaysReturn(1);
        bus.sendMessage(&mockConn1_2.get(), &mockPacket3.get());
        fakeit::Verify(Method(mockPacket3, isTypeBroadcast)).Once();
        fakeit::Verify(Method(mockPacket3, getRecipientType)).Once();
        fakeit::Verify(Method(mockPacket3, isInstanceBroadcast)).Once();
        fakeit::Verify(Method(mockPacket3, getRecipientInstance)).Once();
        fakeit::Verify(Method(mockConn1_1_1, sendMessage).Using(&mockConn1_2.get(), &mockPacket3.get())).Once();
        fakeit::Verify(Method(mockConn1_1_3, sendMessage).Using(&mockConn1_2.get(), &mockPacket3.get())).Once();
        verifyAllMocksChecked();

        // Remove another duplicate and make sure that it ceases to receive the messages
        bus.disconnected(&mockConn1_1_3.get());
        fakeit::Verify(Method(mockConn1_1_3, getType)).Exactly(2);
        fakeit::Verify(Method(mockConn1_1_3, getInstance)).Exactly(2);

        fakeit::When(Method(mockPacket4, isTypeBroadcast)).AlwaysReturn(false);
        fakeit::When(Method(mockPacket4, getRecipientType)).AlwaysReturn(1);
        fakeit::When(Method(mockPacket4, isInstanceBroadcast)).AlwaysReturn(false);
        fakeit::When(Method(mockPacket4, getRecipientInstance)).AlwaysReturn(1);
        bus.sendMessage(&mockConn1_2.get(), &mockPacket4.get());
        fakeit::Verify(Method(mockPacket4, isTypeBroadcast)).Once();
        fakeit::Verify(Method(mockPacket4, getRecipientType)).Once();
        fakeit::Verify(Method(mockPacket4, isInstanceBroadcast)).Once();
        fakeit::Verify(Method(mockPacket4, getRecipientInstance)).Once();
        fakeit::Verify(Method(mockConn1_1_1, sendMessage).Using(&mockConn1_2.get(), &mockPacket4.get())).Once();
        verifyAllMocksChecked();

        // Remove last duplicate and make sure that there is no recipient
        bus.disconnected(&mockConn1_1_1.get());
        fakeit::Verify(Method(mockConn1_1_1, getType)).Exactly(2);
        fakeit::Verify(Method(mockConn1_1_1, getInstance)).Exactly(2);

        fakeit::When(Method(mockPacket5, isTypeBroadcast)).AlwaysReturn(false);
        fakeit::When(Method(mockPacket5, getRecipientType)).AlwaysReturn(1);
        fakeit::When(Method(mockPacket5, isInstanceBroadcast)).AlwaysReturn(false);
        fakeit::When(Method(mockPacket5, getRecipientInstance)).AlwaysReturn(1);
        bus.sendMessage(&mockConn1_2.get(), &mockPacket5.get());
        fakeit::Verify(Method(mockPacket5, isTypeBroadcast)).Once();
        fakeit::Verify(Method(mockPacket5, getRecipientType)).Once();
        fakeit::Verify(Method(mockPacket5, isInstanceBroadcast)).Once();
        fakeit::Verify(Method(mockPacket5, getRecipientInstance)).Once();
        verifyAllMocksChecked();
    }

    /**
     * Verify that when broadcast, all but the sender receive the message
     */
    BOOST_FIXTURE_TEST_CASE(ConnectTwoAndSendMessageTestBroadcastAll, BusTest::TestFixtureAllConnectionsConnected) {
        // In this test the messages are to be broadcast
        fakeit::When(Method(mockPacket1, isTypeBroadcast)).AlwaysReturn(true);
        fakeit::When(Method(mockPacket1, isInstanceBroadcast)).AlwaysReturn(true);
        fakeit::When(Method(mockPacket2, isTypeBroadcast)).AlwaysReturn(true);
        fakeit::When(Method(mockPacket2, isInstanceBroadcast)).AlwaysReturn(true);

        // Send from connection 1
        bus.sendMessage(&mockConn1_1_1.get(), &mockPacket1.get());
        fakeit::Verify(Method(mockPacket1, isTypeBroadcast)).Once();
        fakeit::Verify(Method(mockPacket1, isInstanceBroadcast)).Once();
        fakeit::Verify(Method(mockConn1_1_2, sendMessage).Using(&mockConn1_1_1.get(), &mockPacket1.get())).Once();
        fakeit::Verify(Method(mockConn1_1_3, sendMessage).Using(&mockConn1_1_1.get(), &mockPacket1.get())).Once();
        fakeit::Verify(Method(mockConn1_2, sendMessage).Using(&mockConn1_1_1.get(), &mockPacket1.get())).Once();
        fakeit::Verify(Method(mockConn2_1, sendMessage).Using(&mockConn1_1_1.get(), &mockPacket1.get())).Once();
        fakeit::Verify(Method(mockConn2_2, sendMessage).Using(&mockConn1_1_1.get(), &mockPacket1.get())).Once();
        fakeit::Verify(Method(mockConn3_1, sendMessage).Using(&mockConn1_1_1.get(), &mockPacket1.get())).Once();
        fakeit::Verify(Method(mockConn3_2, sendMessage).Using(&mockConn1_1_1.get(), &mockPacket1.get())).Once();
        verifyAllMocksChecked();

        // Send from connection 2
        bus.sendMessage(&mockConn2_2.get(), &mockPacket2.get());
        fakeit::Verify(Method(mockPacket2, isTypeBroadcast)).Once();
        fakeit::Verify(Method(mockPacket2, isInstanceBroadcast)).Once();
        fakeit::Verify(Method(mockConn1_1_1, sendMessage).Using(&mockConn2_2.get(), &mockPacket2.get())).Once();
        fakeit::Verify(Method(mockConn1_1_2, sendMessage).Using(&mockConn2_2.get(), &mockPacket2.get())).Once();
        fakeit::Verify(Method(mockConn1_1_3, sendMessage).Using(&mockConn2_2.get(), &mockPacket2.get())).Once();
        fakeit::Verify(Method(mockConn1_2, sendMessage).Using(&mockConn2_2.get(), &mockPacket2.get())).Once();
        fakeit::Verify(Method(mockConn2_1, sendMessage).Using(&mockConn2_2.get(), &mockPacket2.get())).Once();
        fakeit::Verify(Method(mockConn3_1, sendMessage).Using(&mockConn2_2.get(), &mockPacket2.get())).Once();
        fakeit::Verify(Method(mockConn3_2, sendMessage).Using(&mockConn2_2.get(), &mockPacket2.get())).Once();
        verifyAllMocksChecked();
    }

    /**
     * Verify that when broadcasting to a type, all of the type but the sender receive the message
     */
    BOOST_FIXTURE_TEST_CASE(ConnectTwoAndSendMessageTestBroadcastInstances, BusTest::TestFixtureAllConnectionsConnected) {
        // Send Packet1
        fakeit::When(Method(mockPacket1, isTypeBroadcast)).AlwaysReturn(false);
        fakeit::When(Method(mockPacket1, isInstanceBroadcast)).AlwaysReturn(true);
        fakeit::When(Method(mockPacket1, getRecipientType)).AlwaysReturn(3);

        bus.sendMessage(&mockConn1_1_1.get(), &mockPacket1.get());
        fakeit::Verify(Method(mockPacket1, isTypeBroadcast)).Once();
        fakeit::Verify(Method(mockPacket1, isInstanceBroadcast)).Once();
        fakeit::Verify(Method(mockPacket1, getRecipientType)).Once();
        // Since a type 1 connection is sending all type 3 connections receive it
        fakeit::Verify(Method(mockConn3_1, sendMessage).Using(&mockConn1_1_1.get(), &mockPacket1.get())).Once();
        fakeit::Verify(Method(mockConn3_2, sendMessage).Using(&mockConn1_1_1.get(), &mockPacket1.get())).Once();
        verifyAllMocksChecked();

        // Send Packet2
        fakeit::When(Method(mockPacket2, isTypeBroadcast)).AlwaysReturn(false);
        fakeit::When(Method(mockPacket2, isInstanceBroadcast)).AlwaysReturn(true);
        fakeit::When(Method(mockPacket2, getRecipientType)).AlwaysReturn(2);

        bus.sendMessage(&mockConn2_2.get(), &mockPacket2.get());
        fakeit::Verify(Method(mockPacket2, isTypeBroadcast)).Once();
        fakeit::Verify(Method(mockPacket2, isInstanceBroadcast)).Once();
        fakeit::Verify(Method(mockPacket2, getRecipientType)).Once();
        // Since a type 2 connection is sending, then only the non-sender type 2 receives it
        fakeit::Verify(Method(mockConn2_1, sendMessage).Using(&mockConn2_2.get(), &mockPacket2.get())).Once();
        verifyAllMocksChecked();
    }

    /**
     * Verify that when broadcasting to an instance, all of the instances receive except for the sender
     */
    BOOST_FIXTURE_TEST_CASE(ConnectTwoAndSendMessageTestBroadcastType, BusTest::TestFixtureAllConnectionsConnected) {
        // Send Packet1
        fakeit::When(Method(mockPacket1, isTypeBroadcast)).AlwaysReturn(true);
        fakeit::When(Method(mockPacket1, isInstanceBroadcast)).AlwaysReturn(false);
        fakeit::When(Method(mockPacket1, getRecipientInstance)).AlwaysReturn(2);

        bus.sendMessage(&mockConn1_1_1.get(), &mockPacket1.get());
        fakeit::Verify(Method(mockPacket1, isTypeBroadcast)).Once();
        fakeit::Verify(Method(mockPacket1, isInstanceBroadcast)).Once();
        fakeit::Verify(Method(mockPacket1, getRecipientInstance)).Once();
        // Since an instance 1 connection is sending all instance 2 connections receive it
        fakeit::Verify(Method(mockConn1_2, sendMessage).Using(&mockConn1_1_1.get(), &mockPacket1.get())).Once();
        fakeit::Verify(Method(mockConn2_2, sendMessage).Using(&mockConn1_1_1.get(), &mockPacket1.get())).Once();
        fakeit::Verify(Method(mockConn3_2, sendMessage).Using(&mockConn1_1_1.get(), &mockPacket1.get())).Once();
        verifyAllMocksChecked();

        // Send Packet2
        fakeit::When(Method(mockPacket2, isTypeBroadcast)).AlwaysReturn(true);
        fakeit::When(Method(mockPacket2, isInstanceBroadcast)).AlwaysReturn(false);
        fakeit::When(Method(mockPacket2, getRecipientInstance)).AlwaysReturn(1);

        bus.sendMessage(&mockConn3_1.get(), &mockPacket2.get());
        fakeit::Verify(Method(mockPacket2, isTypeBroadcast)).Once();
        fakeit::Verify(Method(mockPacket2, isInstanceBroadcast)).Once();
        fakeit::Verify(Method(mockPacket2, getRecipientInstance)).Once();
        // Since an instance 1 connection is sending only the non-sender instance 1's will receive it
        fakeit::Verify(Method(mockConn1_1_1, sendMessage).Using(&mockConn3_1.get(), &mockPacket2.get())).Once();
        fakeit::Verify(Method(mockConn2_1, sendMessage).Using(&mockConn3_1.get(), &mockPacket2.get())).Once();
        verifyAllMocksChecked();
    }

    /**
     * Verify that when not broadcasting, only the recipient receives the message
     */
    BOOST_FIXTURE_TEST_CASE(ConnectTwoAndSendMessageTestNoBroadcast, BusTest::TestFixtureAllConnectionsConnected) {
        // Send Packet1
        fakeit::When(Method(mockPacket1, isTypeBroadcast)).AlwaysReturn(false);
        fakeit::When(Method(mockPacket1, isInstanceBroadcast)).AlwaysReturn(false);
        fakeit::When(Method(mockPacket1, getRecipientType)).AlwaysReturn(1);
        fakeit::When(Method(mockPacket1, getRecipientInstance)).AlwaysReturn(2);

        bus.sendMessage(&mockConn1_1_1.get(), &mockPacket1.get());
        fakeit::Verify(Method(mockPacket1, isTypeBroadcast)).Once();
        fakeit::Verify(Method(mockPacket1, isInstanceBroadcast)).Once();
        fakeit::Verify(Method(mockPacket1, getRecipientType)).Once();
        fakeit::Verify(Method(mockPacket1, getRecipientInstance)).Once();
        fakeit::Verify(Method(mockConn1_2, sendMessage).Using(&mockConn1_1_1.get(), &mockPacket1.get())).Once();
        verifyAllMocksChecked();

        // Send Packet2
        fakeit::When(Method(mockPacket2, isTypeBroadcast)).AlwaysReturn(false);
        fakeit::When(Method(mockPacket2, isInstanceBroadcast)).AlwaysReturn(false);
        fakeit::When(Method(mockPacket2, getRecipientType)).AlwaysReturn(2);
        fakeit::When(Method(mockPacket2, getRecipientInstance)).AlwaysReturn(1);

        bus.sendMessage(&mockConn3_1.get(), &mockPacket2.get());
        fakeit::Verify(Method(mockPacket2, isTypeBroadcast)).Once();
        fakeit::Verify(Method(mockPacket2, isInstanceBroadcast)).Once();
        fakeit::Verify(Method(mockPacket2, getRecipientType)).Once();
        fakeit::Verify(Method(mockPacket2, getRecipientInstance)).Once();
        fakeit::Verify(Method(mockConn2_1, sendMessage).Using(&mockConn3_1.get(), &mockPacket2.get())).Once();
        verifyAllMocksChecked();
    }

    /**
     * Verify that can connect three, sender does not receive the message. After disconnecting the disconnected
     * no longer can receive messages.
     */
    BOOST_FIXTURE_TEST_CASE(ConnectThreeDisconnectOneAndSendMessageTest, BusTest::TestFixtureAllConnectionsConnected) {
        // With all connected, a send packet is received by all
        fakeit::When(Method(mockPacket1, isTypeBroadcast)).AlwaysReturn(true);
        fakeit::When(Method(mockPacket1, isInstanceBroadcast)).AlwaysReturn(true);
        bus.sendMessage(&mockConn1_1_1.get(), &mockPacket1.get());
        fakeit::Verify(Method(mockPacket1, isTypeBroadcast)).Once();
        fakeit::Verify(Method(mockPacket1, isInstanceBroadcast)).Once();
        fakeit::Verify(Method(mockConn1_1_2, sendMessage).Using(&mockConn1_1_1.get(), &mockPacket1.get())).Once();
        fakeit::Verify(Method(mockConn1_1_3, sendMessage).Using(&mockConn1_1_1.get(), &mockPacket1.get())).Once();
        fakeit::Verify(Method(mockConn1_2, sendMessage).Using(&mockConn1_1_1.get(), &mockPacket1.get())).Once();
        fakeit::Verify(Method(mockConn2_1, sendMessage).Using(&mockConn1_1_1.get(), &mockPacket1.get())).Once();
        fakeit::Verify(Method(mockConn2_2, sendMessage).Using(&mockConn1_1_1.get(), &mockPacket1.get())).Once();
        fakeit::Verify(Method(mockConn3_1, sendMessage).Using(&mockConn1_1_1.get(), &mockPacket1.get())).Once();
        fakeit::Verify(Method(mockConn3_2, sendMessage).Using(&mockConn1_1_1.get(), &mockPacket1.get())).Once();
        verifyAllMocksChecked();

        // Disconnect a connection, and it will no longer receive the message
        bus.disconnected(&mockConn3_1.get());
        fakeit::Verify(Method(mockConn3_1, getType)).Exactly(2);
        fakeit::Verify(Method(mockConn3_1, getInstance)).Exactly(2);

        // Now when sending all but the disconnected with receive the message
        fakeit::When(Method(mockPacket2, isTypeBroadcast)).AlwaysReturn(true);
        fakeit::When(Method(mockPacket2, isInstanceBroadcast)).AlwaysReturn(true);
        bus.sendMessage(&mockConn1_1_1.get(), &mockPacket2.get());
        fakeit::Verify(Method(mockPacket2, isTypeBroadcast)).Once();
        fakeit::Verify(Method(mockPacket2, isInstanceBroadcast)).Once();
        fakeit::Verify(Method(mockConn1_1_2, sendMessage).Using(&mockConn1_1_1.get(), &mockPacket2.get())).Once();
        fakeit::Verify(Method(mockConn1_1_3, sendMessage).Using(&mockConn1_1_1.get(), &mockPacket2.get())).Once();
        fakeit::Verify(Method(mockConn1_2, sendMessage).Using(&mockConn1_1_1.get(), &mockPacket2.get())).Once();
        fakeit::Verify(Method(mockConn2_1, sendMessage).Using(&mockConn1_1_1.get(), &mockPacket2.get())).Once();
        fakeit::Verify(Method(mockConn2_2, sendMessage).Using(&mockConn1_1_1.get(), &mockPacket2.get())).Once();
        fakeit::Verify(Method(mockConn3_2, sendMessage).Using(&mockConn1_1_1.get(), &mockPacket2.get())).Once();
        verifyAllMocksChecked();

        // Disconnect another connection, and it will no longer receive the message
        bus.disconnected(&mockConn2_2.get());
        fakeit::Verify(Method(mockConn2_2, getType)).Exactly(2);
        fakeit::Verify(Method(mockConn2_2, getInstance)).Exactly(2);

        // Now when sending all but the disconnected with receive the message
        fakeit::When(Method(mockPacket3, isTypeBroadcast)).AlwaysReturn(true);
        fakeit::When(Method(mockPacket3, isInstanceBroadcast)).AlwaysReturn(true);
        bus.sendMessage(&mockConn1_1_1.get(), &mockPacket3.get());
        fakeit::Verify(Method(mockPacket3, isTypeBroadcast)).Once();
        fakeit::Verify(Method(mockPacket3, isInstanceBroadcast)).Once();
        fakeit::Verify(Method(mockConn1_1_2, sendMessage).Using(&mockConn1_1_1.get(), &mockPacket3.get())).Once();
        fakeit::Verify(Method(mockConn1_1_3, sendMessage).Using(&mockConn1_1_1.get(), &mockPacket3.get())).Once();
        fakeit::Verify(Method(mockConn1_2, sendMessage).Using(&mockConn1_1_1.get(), &mockPacket3.get())).Once();
        fakeit::Verify(Method(mockConn2_1, sendMessage).Using(&mockConn1_1_1.get(), &mockPacket3.get())).Once();
        fakeit::Verify(Method(mockConn3_2, sendMessage).Using(&mockConn1_1_1.get(), &mockPacket3.get())).Once();
        verifyAllMocksChecked();
    }

    BOOST_AUTO_TEST_SUITE_END()
