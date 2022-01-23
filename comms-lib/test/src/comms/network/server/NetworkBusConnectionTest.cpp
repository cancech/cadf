#define BOOST_TEST_DYN_LINK
#ifdef STAND_ALONE
#   define BOOST_TEST_MODULE Main
#endif
#include <boost/test/unit_test.hpp>
#include <fakeit.hpp>

#include "comms/network/server/NetworkBusConnection.h"
#include "comms/bus/Bus.h"
#include "comms/connection/ConnectionException.h"

namespace NetworkBusConnectionTest {

    /**
     * Test override that exposes the messageReceived method
     */
    class TestNetworkBusConnection: public cadf::comms::NetworkBusConnection {
        public:
            TestNetworkBusConnection(cadf::comms::IConnection *connection): NetworkBusConnection(connection) {
            }

            virtual void messageReceived(const cadf::comms::MessagePacket *packet) {
                NetworkBusConnection::messageReceived(packet);
            }
    };

    /*
     * Helper fixture for initializing and preparing all of the necessary mocks
     */
    struct SetupMocks {

            SetupMocks() {
                fakeit::When(Method(mockConnection, getType)).AlwaysReturn(78);
                fakeit::When(Method(mockConnection, getInstance)).AlwaysReturn(583);
                fakeit::Fake(Method(mockConnection, sendPacket));
                fakeit::Fake(Method(mockConnection, addMessageListener));
                fakeit::Fake(Method(mockConnection, removeMessageListener));
                fakeit::When(Method(mockConnection, disconnect)).AlwaysReturn(true);

                fakeit::Fake(Method(mockBus, connected));
                fakeit::Fake(Method(mockBus, disconnected));
                fakeit::Fake(Method(mockBus, sendMessage));
            }

            ~SetupMocks() {
                verifyAllMocksChecked();
            }

            void verifyAllMocksChecked() {
                fakeit::VerifyNoOtherInvocations(mockConnection, mockBus, mockPacket);
            }

            // Mocks to use for testing
            fakeit::Mock<cadf::comms::IConnection> mockConnection;
            fakeit::Mock<cadf::comms::IBus> mockBus;
            fakeit::Mock<cadf::comms::IBus> mockBus2;
            fakeit::Mock<cadf::comms::MessagePacket> mockPacket;
    };

    /**
     * Helper fixture to initialize the NetworkBusConnection instance that is to be used for the tests
     */
    struct TestFixture: public SetupMocks {

            TestFixture() : SetupMocks() {
                conn = new NetworkBusConnectionTest::TestNetworkBusConnection(&mockConnection.get());
                fakeit::Verify(Method(mockConnection, addMessageListener).Using(conn)).Once();
                verifyAllMocksChecked();
            }

            ~TestFixture() {
                delete (conn);
                fakeit::Verify(Method(mockConnection, removeMessageListener).Using(conn)).Once();

                if (isBusRegistered) {
                    fakeit::Verify(Method(mockConnection, disconnect)).Once();
                    fakeit::Verify(Method(mockBus, disconnected).Using(conn)).Once();
                }
                verifyAllMocksChecked();
            }

            void registerTheBus() {
                conn->registerBus(&mockBus.get());
                fakeit::Verify(Method(mockBus, connected).Using(conn)).Once();
                isBusRegistered = true;
            }

            void disconnectFromBus(int timesDisconnectAttempted) {
                conn->disconnect();
                fakeit::Verify(Method(mockConnection, disconnect)).Exactly(timesDisconnectAttempted);
                fakeit::Verify(Method(mockBus, disconnected).Using(conn)).Once();
                isBusRegistered = false;
            }

            bool isBusRegistered = false;

            /** The connection to test */
            NetworkBusConnectionTest::TestNetworkBusConnection *conn;
    };

}

BOOST_AUTO_TEST_SUITE(NetworkBusConnection_Test_Suite)

/**
 * Verify that methods that are independent of the state work as expected
 */
    BOOST_FIXTURE_TEST_CASE(VerifySimpleValuesTest, NetworkBusConnectionTest::TestFixture) {
        BOOST_CHECK_EQUAL(78, conn->getType());
        fakeit::Verify(Method(mockConnection, getType)).Once();

        BOOST_CHECK_EQUAL(583, conn->getInstance());
        fakeit::Verify(Method(mockConnection, getInstance)).Once();

        conn->sendMessage(NULL, &mockPacket.get());
        fakeit::Verify(Method(mockConnection, sendPacket).Using(&mockPacket.get())).Once();
    }

    /**
     * Verify that a Bus can be registered properly
     */
    BOOST_FIXTURE_TEST_CASE(RegisterBusTest, NetworkBusConnectionTest::TestFixture) {
        // Register the bus
        registerTheBus();

        // Attempting to register another bus fails
        BOOST_CHECK_THROW(conn->registerBus(&mockBus.get()), cadf::comms::BusException);
        BOOST_CHECK_THROW(conn->registerBus(&mockBus2.get()), cadf::comms::BusException);
    }

    /**
     * Verify that a Bus can be disconnected properly
     */
    BOOST_FIXTURE_TEST_CASE(DisconnectSuccessfulTest, RegisterBusTest) {

        // Cannot disconnect when not registered
        BOOST_CHECK_THROW(conn->disconnect(), cadf::comms::BusException);

        // Register the bus
        RegisterBusTest::test_method();

        // Disconnect from the bus
        disconnectFromBus(1);

        // Cannot disconnect again
        BOOST_CHECK_THROW(conn->disconnect(), cadf::comms::BusException);
        BOOST_CHECK_THROW(conn->disconnect(), cadf::comms::BusException);
    }

    /**
     * Verify that a Bus can be disconnected properly
     */
    BOOST_FIXTURE_TEST_CASE(DisconnectFailureTest, RegisterBusTest) {
        RegisterBusTest::test_method();
        fakeit::When(Method(mockConnection, disconnect)).AlwaysReturn(false);

        BOOST_CHECK(!conn->disconnect());
        fakeit::Verify(Method(mockConnection, disconnect)).Once();

        // Return to normal operation
        fakeit::When(Method(mockConnection, disconnect)).AlwaysReturn(true);
        disconnectFromBus(2);
    }

    /**
     * Verify that a Bus can properly distribute messages
     */
    BOOST_FIXTURE_TEST_CASE(MessageReceivedWhenBusRegisteredTest, RegisterBusTest) {
        RegisterBusTest::test_method();
        conn->messageReceived(&mockPacket.get());
        fakeit::Verify(Method(mockBus, sendMessage).Using(conn, &mockPacket.get())).Once();
    }

    /**
     * Verify that a Bus can properly distribute messages
     */
    BOOST_FIXTURE_TEST_CASE(MessageReceivedWhenBusNotRegisteredTest, NetworkBusConnectionTest::TestFixture) {
        conn->messageReceived(&mockPacket.get());
    }

    BOOST_AUTO_TEST_SUITE_END()
