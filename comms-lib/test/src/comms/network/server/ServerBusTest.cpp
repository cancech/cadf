#define BOOST_TEST_DYN_LINK
#ifdef STAND_ALONE
#   define BOOST_TEST_MODULE Main
#endif
#include <boost/test/unit_test.hpp>
#include <fakeit.hpp>
#include <iostream>

#include "comms/network/server/ServerBus.h"

namespace ServerBusTest {

    class TestServerBus: public cadf::comms::ServerBus {
        public:
            TestServerBus(cadf::comms::ISocket *socket, cadf::comms::IBus *bus) : ServerBus(socket, bus) {
            }

            cadf::comms::IBusConnection* getBusConnection(cadf::comms::IConnection *conn) {
                auto iter = m_busConnections.find(conn);
                if (iter == m_busConnections.end())
                    return NULL;

                return iter->second;
            }

            void verifyNumOfConnections(size_t expected) {
                BOOST_CHECK_EQUAL(expected, m_busConnections.size());
            }
    };

    /*
     * Helper fixture for initializing and preparing all of the necessary mocks
     */
    struct SetupMocks {

            SetupMocks() {
                fakeit::Fake(Method(mockBus, connected));
                fakeit::Fake(Method(mockBus, disconnected));
                fakeit::Fake(Method(mockConn1, addMessageListener));
                fakeit::Fake(Method(mockConn2, addMessageListener));
                fakeit::Fake(Method(mockConn3, addMessageListener));
                fakeit::Fake(Method(mockConn1, removeMessageListener));
                fakeit::Fake(Method(mockConn2, removeMessageListener));
                fakeit::Fake(Method(mockConn3, removeMessageListener));
                fakeit::When(Method(mockConn1, disconnect)).AlwaysReturn(true);
                fakeit::When(Method(mockConn2, disconnect)).AlwaysReturn(true);
                fakeit::When(Method(mockConn3, disconnect)).AlwaysReturn(true);
            }

            ~SetupMocks() {
                verifyAllMocksChecked();
            }

            void verifyAllMocksChecked() {
                fakeit::VerifyNoOtherInvocations(mockSocket, mockBus, mockConn1, mockConn2, mockConn3);
            }

            // Mocks to use for testing
            fakeit::Mock<cadf::comms::ISocket> mockSocket;
            fakeit::Mock<cadf::comms::IBus> mockBus;
            fakeit::Mock<cadf::comms::IConnection> mockConn1;
            fakeit::Mock<cadf::comms::IConnection> mockConn2;
            fakeit::Mock<cadf::comms::IConnection> mockConn3;
    };

    /**
     * Helper fixture to initialize the BasicServerConnection instance that is to be used for the tests
     */
    struct TestFixture: public SetupMocks {

            TestFixture() : SetupMocks(), server(&mockSocket.get(), &mockBus.get()) {
                verifyAllMocksChecked();
            }

            ~TestFixture() {
                verifyAllMocksChecked();
            }

            void addConnection(fakeit::Mock<cadf::comms::IConnection> &mockConn, size_t expectedNumConnections) {
                server.clientConnected(&mockConn.get());
                server.verifyNumOfConnections(expectedNumConnections);
                cadf::comms::IBusConnection *busConn = server.getBusConnection(&mockConn.get());
                BOOST_CHECK(busConn);
                fakeit::Verify(Method(mockConn, addMessageListener).Using(dynamic_cast<cadf::comms::IMessageListener*>(busConn))).Once();
                fakeit::Verify(Method(mockBus, connected).Using(busConn)).Once();
            }

            void removeConnection(fakeit::Mock<cadf::comms::IConnection> &mockConn, size_t expectedNumConnections) {
                cadf::comms::IBusConnection *busConn = server.getBusConnection(&mockConn.get());
                cadf::comms::IMessageListener *castBusConn = dynamic_cast<cadf::comms::IMessageListener*>(busConn);
                BOOST_CHECK(busConn);
                server.clientDisconnected(&mockConn.get());
                server.verifyNumOfConnections(expectedNumConnections);
                fakeit::Verify(Method(mockConn, removeMessageListener).Using(castBusConn)).Once();
                fakeit::Verify(Method(mockConn, disconnect)).Once();
                fakeit::Verify(Method(mockBus, disconnected).Using(busConn)).Once();
            }

            /** The connection to test */
            TestServerBus server;
    };

}

BOOST_AUTO_TEST_SUITE(ServerBus_Test_Suite)

/**
 * Verify that connections are properly registered with the bus
 */
    BOOST_FIXTURE_TEST_CASE(VerifyConnectionTest, ServerBusTest::TestFixture) {
        server.verifyNumOfConnections(0);
        addConnection(mockConn1, 1);
        addConnection(mockConn2, 2);
        addConnection(mockConn3, 3);
    }

    /**
     * Verify that nothing happens when disconnecting a connection that is not connected
     */
    BOOST_FIXTURE_TEST_CASE(SameConnectionMultipleTimesTest, ServerBusTest::TestFixture) {
        server.verifyNumOfConnections(0);
        addConnection(mockConn1, 1);
        server.clientConnected(&mockConn1.get());
        server.verifyNumOfConnections(1);
        server.clientConnected(&mockConn1.get());
        server.verifyNumOfConnections(1);
        server.clientConnected(&mockConn1.get());
        server.verifyNumOfConnections(1);
    }

    /**
     * Verify that connections are properly deregistered with the bus
     */
    BOOST_FIXTURE_TEST_CASE(VerifyDisconnectionAfterConnectionTest, VerifyConnectionTest) {
        VerifyConnectionTest::test_method();
        server.verifyNumOfConnections(3);
        removeConnection(mockConn2, 2);
        removeConnection(mockConn3, 1);
        removeConnection(mockConn1, 0);
    }

    /**
     * Verify that nothing happens when disconnecting a connection that is not connected
     */
    BOOST_FIXTURE_TEST_CASE(VerifyDisconnectionNotConnectionTest, ServerBusTest::TestFixture) {
        server.verifyNumOfConnections(0);
        server.clientDisconnected(&mockConn1.get());
        server.clientDisconnected(&mockConn2.get());
        server.clientDisconnected(&mockConn3.get());
    }

    BOOST_AUTO_TEST_SUITE_END()
