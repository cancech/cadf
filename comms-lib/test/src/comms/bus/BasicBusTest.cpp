
#define BOOST_TEST_DYN_LINK
#ifdef STAND_ALONE
#   define BOOST_TEST_MODULE Main
#endif
#include <boost/test/unit_test.hpp>
#include <fakeit.hpp>

#include "comms/bus/BasicBus.h"


// Helper classes for the LocalBasicBusTest
namespace LocalBasicBusTest {

    /*
     * Helper fixture for initializing and preparing all of the necessary mocks
     */
    struct SetupMocks {

            SetupMocks() {
                stubMockConnection(mockConn1_1, 1, 1);
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
                fakeit::VerifyNoOtherInvocations(mockConn1_1, mockConn1_2, mockConn2_1, mockConn2_2, mockConn3_1, mockConn3_2, mockPacket1, mockPacket2, mockPacket3);
            }

            fakeit::Mock<cadf::comms::IBusConnection> mockConn1_1;
            fakeit::Mock<cadf::comms::IBusConnection> mockConn1_2;
            fakeit::Mock<cadf::comms::IBusConnection> mockConn2_1;
            fakeit::Mock<cadf::comms::IBusConnection> mockConn2_2;
            fakeit::Mock<cadf::comms::IBusConnection> mockConn3_1;
            fakeit::Mock<cadf::comms::IBusConnection> mockConn3_2;
            fakeit::Mock<cadf::comms::MessagePacket> mockPacket1;
            fakeit::Mock<cadf::comms::MessagePacket> mockPacket2;
            fakeit::Mock<cadf::comms::MessagePacket> mockPacket3;
    };

    struct TestFixtureAllConnectionsConnected: public SetupMocks {

            cadf::comms::BasicBus bus;

            TestFixtureAllConnectionsConnected() {
                // Establish the connections
                connectedMockConnection(mockConn1_1);
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
BOOST_AUTO_TEST_SUITE(LocalBasicBus_Test_Suite)

    /**
     * Verify that when broadcasting to a type, all of the type but the sender receive the message
     */
    BOOST_FIXTURE_TEST_CASE(ConnectTwoAndSendMessageTestBroadcastInstances, LocalBasicBusTest::TestFixtureAllConnectionsConnected) {
        // Send Packet1
        fakeit::When(Method(mockPacket1, isTypeBroadcast)).AlwaysReturn(false);
        fakeit::When(Method(mockPacket1, isInstanceBroadcast)).AlwaysReturn(true);
        fakeit::When(Method(mockPacket1, getRecipientType)).AlwaysReturn(3);

        bus.sendMessage(&mockConn1_1.get(), &mockPacket1.get());
        fakeit::Verify(Method(mockPacket1, isTypeBroadcast)).Once();
        fakeit::Verify(Method(mockPacket1, isInstanceBroadcast)).Once();
        fakeit::Verify(Method(mockPacket1, getRecipientType)).Once();
        // Since a type 1 connection is sending all type 3 connections receive it
        fakeit::Verify(Method(mockConn3_1, sendMessage).Using(&mockConn1_1.get(), &mockPacket1.get())).Once();
        fakeit::Verify(Method(mockConn3_2, sendMessage).Using(&mockConn1_1.get(), &mockPacket1.get())).Once();
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
    BOOST_FIXTURE_TEST_CASE(ConnectTwoAndSendMessageTestBroadcastType, LocalBasicBusTest::TestFixtureAllConnectionsConnected) {
        // Send Packet1
        fakeit::When(Method(mockPacket1, isTypeBroadcast)).AlwaysReturn(true);
        fakeit::When(Method(mockPacket1, isInstanceBroadcast)).AlwaysReturn(false);
        fakeit::When(Method(mockPacket1, getRecipientInstance)).AlwaysReturn(2);

        bus.sendMessage(&mockConn1_1.get(), &mockPacket1.get());
        fakeit::Verify(Method(mockPacket1, isTypeBroadcast)).Once();
        fakeit::Verify(Method(mockPacket1, isInstanceBroadcast)).Once();
        fakeit::Verify(Method(mockPacket1, getRecipientInstance)).Once();
        // Since an instance 1 connection is sending all instance 2 connections receive it
        fakeit::Verify(Method(mockConn1_2, sendMessage).Using(&mockConn1_1.get(), &mockPacket1.get())).Once();
        fakeit::Verify(Method(mockConn2_2, sendMessage).Using(&mockConn1_1.get(), &mockPacket1.get())).Once();
        fakeit::Verify(Method(mockConn3_2, sendMessage).Using(&mockConn1_1.get(), &mockPacket1.get())).Once();
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
        fakeit::Verify(Method(mockConn1_1, sendMessage).Using(&mockConn3_1.get(), &mockPacket2.get())).Once();
        fakeit::Verify(Method(mockConn2_1, sendMessage).Using(&mockConn3_1.get(), &mockPacket2.get())).Once();
        verifyAllMocksChecked();
    }

    /**
     * Verify that when not broadcasting, only the recipient receives the message
     */
    BOOST_FIXTURE_TEST_CASE(ConnectTwoAndSendMessageTestNoBroadcast, LocalBasicBusTest::TestFixtureAllConnectionsConnected) {
        // Send Packet1
        fakeit::When(Method(mockPacket1, isTypeBroadcast)).AlwaysReturn(false);
        fakeit::When(Method(mockPacket1, isInstanceBroadcast)).AlwaysReturn(false);
        fakeit::When(Method(mockPacket1, getRecipientType)).AlwaysReturn(1);
        fakeit::When(Method(mockPacket1, getRecipientInstance)).AlwaysReturn(2);

        bus.sendMessage(&mockConn1_1.get(), &mockPacket1.get());
        fakeit::Verify(Method(mockPacket1, isTypeBroadcast)).Once();
        fakeit::Verify(Method(mockPacket1, isInstanceBroadcast)).Once();
        fakeit::Verify(Method(mockPacket1, getRecipientType)).Once();
        fakeit::Verify(Method(mockPacket1, getRecipientInstance)).Once();
        fakeit::Verify(Method(mockConn1_2, sendMessage).Using(&mockConn1_1.get(), &mockPacket1.get())).Once();
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

    BOOST_AUTO_TEST_SUITE_END()
