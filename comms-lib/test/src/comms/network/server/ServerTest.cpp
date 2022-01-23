#define BOOST_TEST_DYN_LINK
#ifdef STAND_ALONE
#   define BOOST_TEST_MODULE Main
#endif
#include <boost/test/unit_test.hpp>
#include <fakeit.hpp>

#include "comms/network/server/Server.h"

namespace ServerTest {

    class ConcreteServer: public cadf::comms::AbstractServer {
        public:
            ConcreteServer(cadf::comms::ISocket *socket) : AbstractServer(socket) {
            }

            virtual ~ConcreteServer() = default;

            virtual void clientConnected(cadf::comms::IConnection *connection) {
            }

            virtual void clientDisconnected(cadf::comms::IConnection *connection) {
            }
    };

    /*
     * Helper fixture for initializing and preparing all of the necessary mocks
     */
    struct SetupMocks {

            SetupMocks() {
            }

            ~SetupMocks() {
                verifyAllMocksChecked();
            }

            void verifyAllMocksChecked() {
                fakeit::VerifyNoOtherInvocations(mockSocket);
            }

            // Mocks to use for testing
            fakeit::Mock<cadf::comms::ISocket> mockSocket;
    };

    /**
     * Helper fixture to initialize the BasicServerConnection instance that is to be used for the tests
     */
    struct TestFixture: public SetupMocks {

            TestFixture() : SetupMocks(), server(&mockSocket.get()) {
                verifyAllMocksChecked();
            }

            ~TestFixture() {
                verifyAllMocksChecked();
            }

            /** The connection to test */
            ConcreteServer server;
    };

}

BOOST_AUTO_TEST_SUITE(Server_Test_Suite)

/**
 * Verify that AbstractServer just "forwards" the calls to the socket
 */
    BOOST_FIXTURE_TEST_CASE(VerifyConnectTest, ServerTest::TestFixture) {
        fakeit::When(Method(mockSocket, connect)).Return(false).Return(true);
        BOOST_CHECK(!server.start());
        fakeit::Verify(Method(mockSocket, connect)).Once();
        BOOST_CHECK(server.start());
        fakeit::Verify(Method(mockSocket, connect)).Twice();
    }

/**
 * Verify that AbstractServer just "forwards" the calls to the socket
 */
    BOOST_FIXTURE_TEST_CASE(VerifyDisconnectTest, ServerTest::TestFixture) {
        fakeit::When(Method(mockSocket, disconnect)).Return(true).Return(false);
        BOOST_CHECK(server.stop());
        fakeit::Verify(Method(mockSocket, disconnect)).Once();
        BOOST_CHECK(!server.stop());
        fakeit::Verify(Method(mockSocket, disconnect)).Twice();
    }

    /**
     * Verify that AbstractServer just "forwards" the calls to the socket
     */
        BOOST_FIXTURE_TEST_CASE(VerifyOnlineTest, ServerTest::TestFixture) {
            fakeit::When(Method(mockSocket, isConnected)).Return(false).Return(true);
            BOOST_CHECK(!server.isOnline());
            fakeit::Verify(Method(mockSocket, isConnected)).Once();
            BOOST_CHECK(server.isOnline());
            fakeit::Verify(Method(mockSocket, isConnected)).Twice();
        }

    BOOST_AUTO_TEST_SUITE_END()
