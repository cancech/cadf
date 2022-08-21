
#define BOOST_TEST_DYN_LINK
#ifdef STAND_ALONE
#   define BOOST_TEST_MODULE Main
#endif
#include <boost/test/unit_test.hpp>

#include "comms/bus/ThreadedBus.h"
#include "comms/Constants.h"
#include "TestMessage.h"

#include <fakeit.hpp>

// Helper classes for the LocalThreadedBusTest
namespace LocalThreadedBusTest {

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
                fakeit::When(Method(mockThreadPool, schedule)).AlwaysDo([=] (std::function<void()> func) {scheduledFunc = func;});
            }

            void stubMockConnection(fakeit::Mock<cadf::comms::IBusConnection> &mockConnection, int type, int instance) {
                fakeit::When(Method(mockConnection, getType)).AlwaysReturn(type);
                fakeit::When(Method(mockConnection, getInstance)).AlwaysReturn(instance);
                fakeit::When(Method(mockConnection, sendMessage)).AlwaysReturn();
            }

            cadf::comms::MessagePacket* mockPacket(fakeit::Mock<cadf::comms::MessagePacket> &mockPacket, int type, int instance) {
                cadf::comms::MessagePacket *clonePacket = new cadf::comms::MessagePacket(new TestMessage1(), type, instance);
                fakeit::When(Method(mockPacket, clone)).AlwaysReturn(clonePacket);
                fakeit::When(Method(mockPacket, getRecipientType)).AlwaysReturn(type);
                fakeit::When(Method(mockPacket, getRecipientInstance)).AlwaysReturn(instance);
                return clonePacket;
            }

            ~SetupMocks() {
                verifyAllMocksChecked();
            }

            void verifyAllMocksChecked() {
                fakeit::VerifyNoOtherInvocations(mockThreadPool, mockConn1_1, mockConn1_2, mockConn2_1, mockConn2_2, mockConn3_1, mockConn3_2, mockPacket1, mockPacket2, mockPacket3);
            }

            fakeit::Mock<cadf::thread::IThreadPool> mockThreadPool;
            std::function<void()> scheduledFunc;
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

            cadf::comms::ThreadedBus bus;

            TestFixtureAllConnectionsConnected() : SetupMocks(), bus(&mockThreadPool.get()) {
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
BOOST_AUTO_TEST_SUITE(LocalThreadedBus_Test_Suite)

/**
 * Verify that when broadcasting to a type, all of the type but the sender receive the message
 */
    BOOST_FIXTURE_TEST_CASE(ConnectTwoAndSendMessageTestBroadcastInstances, LocalThreadedBusTest::TestFixtureAllConnectionsConnected) {
        // Send Packet1
        cadf::comms::MessagePacket *clonePacket = mockPacket(mockPacket1, 3, cadf::comms::ConnectionConstants::BROADCAST);

        // Sending the message schedules with the thread pool
        bus.sendMessage(&mockConn1_1.get(), &mockPacket1.get());
        fakeit::Verify(Method(mockPacket1, clone)).Once();
        fakeit::Verify(Method(mockThreadPool, schedule)).Once();

        // Trigger the "thread" call
        scheduledFunc();
        // Since a type 1 connection is sending all type 3 connections receive it
        fakeit::Verify(Method(mockConn3_1, sendMessage).Using(&mockConn1_1.get(), clonePacket)).Once();
        fakeit::Verify(Method(mockConn3_2, sendMessage).Using(&mockConn1_1.get(), clonePacket)).Once();
        verifyAllMocksChecked();

        //====================
        // Send Packet2
        clonePacket = mockPacket(mockPacket2, 2, cadf::comms::ConnectionConstants::BROADCAST);

        // Sending the message schedules with the thread pool
        bus.sendMessage(&mockConn2_2.get(), &mockPacket2.get());
        fakeit::Verify(Method(mockPacket2, clone)).Once();
        fakeit::Verify(Method(mockThreadPool, schedule)).Twice();

        // Trigger the "thread" call
        scheduledFunc();
        // Since a type 2 connection is sending, then only the non-sender type 2 receives it
        fakeit::Verify(Method(mockConn2_1, sendMessage).Using(&mockConn2_2.get(), clonePacket)).Once();
        verifyAllMocksChecked();
    }

    /**
     * Verify that when broadcasting to an instance, all of the instances receive except for the sender
     */
    BOOST_FIXTURE_TEST_CASE(ConnectTwoAndSendMessageTestBroadcastType, LocalThreadedBusTest::TestFixtureAllConnectionsConnected) {
        // Send Packet1
        cadf::comms::MessagePacket *clonePacket = mockPacket(mockPacket1, cadf::comms::ConnectionConstants::BROADCAST, 2);

        // Sending the message schedules with the thread pool
        bus.sendMessage(&mockConn1_1.get(), &mockPacket1.get());
        fakeit::Verify(Method(mockPacket1, clone)).Once();
        fakeit::Verify(Method(mockThreadPool, schedule)).Once();

        // Trigger the "thread" call
        scheduledFunc();
        // Since an instance 1 connection is sending all instance 2 connections receive it
        fakeit::Verify(Method(mockConn1_2, sendMessage).Using(&mockConn1_1.get(), clonePacket)).Once();
        fakeit::Verify(Method(mockConn2_2, sendMessage).Using(&mockConn1_1.get(), clonePacket)).Once();
        fakeit::Verify(Method(mockConn3_2, sendMessage).Using(&mockConn1_1.get(), clonePacket)).Once();
        verifyAllMocksChecked();

        //====================
        // Send Packet2
        clonePacket = mockPacket(mockPacket2, cadf::comms::ConnectionConstants::BROADCAST, 1);

        // Sending the message schedules with the thread pool
        bus.sendMessage(&mockConn3_1.get(), &mockPacket2.get());
        fakeit::Verify(Method(mockPacket2, clone)).Once();
        fakeit::Verify(Method(mockThreadPool, schedule)).Twice();

        // Trigger the "thread" call
        scheduledFunc();
        // Since an instance 1 connection is sending, then only the non-sender type 1 receives it
        fakeit::Verify(Method(mockConn1_1, sendMessage).Using(&mockConn3_1.get(), clonePacket)).Once();
        fakeit::Verify(Method(mockConn2_1, sendMessage).Using(&mockConn3_1.get(), clonePacket)).Once();
        verifyAllMocksChecked();
    }

    /**
     * Verify that when not broadcasting, only the recipient receives the message
     */
    BOOST_FIXTURE_TEST_CASE(ConnectTwoAndSendMessageTestNoBroadcast, LocalThreadedBusTest::TestFixtureAllConnectionsConnected) {
        // Send Packet1
        cadf::comms::MessagePacket *clonePacket = mockPacket(mockPacket1, 1, 2);

        // Sending the message schedules with the thread pool
        bus.sendMessage(&mockConn1_1.get(), &mockPacket1.get());
        fakeit::Verify(Method(mockPacket1, clone)).Once();
        fakeit::Verify(Method(mockThreadPool, schedule)).Once();

        // Trigger the "thread" call
        scheduledFunc();
        fakeit::Verify(Method(mockConn1_2, sendMessage).Using(&mockConn1_1.get(), clonePacket)).Once();
        verifyAllMocksChecked();

        //====================
        // Send Packet2
        clonePacket = mockPacket(mockPacket2, 2, 1);

        // Sending the message schedules with the thread pool
        bus.sendMessage(&mockConn3_1.get(), &mockPacket2.get());
        fakeit::Verify(Method(mockPacket2, clone)).Once();
        fakeit::Verify(Method(mockThreadPool, schedule)).Twice();

        // Trigger the "thread" call
        scheduledFunc();
        fakeit::Verify(Method(mockConn2_1, sendMessage).Using(&mockConn3_1.get(), clonePacket)).Once();
        verifyAllMocksChecked();
    }

    BOOST_AUTO_TEST_SUITE_END()
