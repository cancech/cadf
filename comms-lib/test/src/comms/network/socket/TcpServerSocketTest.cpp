#define BOOST_TEST_DYN_LINK
#ifdef STAND_ALONE
#   define BOOST_TEST_MODULE Main
#endif
#include <boost/test/unit_test.hpp>
#include <fakeit.hpp>

#include "comms/network/socket/TcpServerSocket.h"

namespace TcpServerSocketTest {

    cadf::comms::NetworkInfo serverInfo() {
        cadf::comms::NetworkInfo serverNetInfo;
        serverNetInfo.ipVersion = cadf::comms::NetworkInfo::IPv4;
        serverNetInfo.netAddress = "127.0.0.1";
        serverNetInfo.port = 101010;
        return serverNetInfo;
    }

    /*
     * Helper fixture for initializing and preparing all of the necessary mocks
     */
    struct SetupMocks {

            SetupMocks() {
                fakeit::When(Method(mockConnectionHandler, purge)).AlwaysReturn();
            }

            ~SetupMocks() {
                verifyAllMocksChecked();
            }

            void verifyAllMocksChecked() {
                fakeit::VerifyNoOtherInvocations(mockConnectionHandler, mockOutputBuffer, mockListener);
            }

            // Mocks to use for testing
            fakeit::Mock<cadf::comms::IServerConnectionHandler> mockConnectionHandler;
            fakeit::Mock<cadf::comms::OutputBuffer> mockOutputBuffer;
            fakeit::Mock<cadf::comms::ISocketMessageReceivedListener> mockListener;
    };

    struct TestFixture: public SetupMocks {

            TestFixture() {
                m_socket = new cadf::comms::TcpServerSocket(TcpServerSocketTest::serverInfo(), &mockConnectionHandler.get(), 10);
                m_socket->addMessageListener(&mockListener.get());
            }

            ~TestFixture() {
                delete (m_socket);
            }

            cadf::comms::TcpServerSocket *m_socket;
    };
}

/**
 * Test suite for the TcpServerSocket
 */
BOOST_AUTO_TEST_SUITE(TcpServerSocket_Test_Suite)

/**
 * Verify that can start and stop the server
 */
    BOOST_FIXTURE_TEST_CASE(StartAndStopServerTest, TcpServerSocketTest::TestFixture) {
        BOOST_CHECK(!m_socket->isConnected());
        BOOST_CHECK(m_socket->connect());
        BOOST_CHECK(m_socket->isConnected());
        BOOST_CHECK(m_socket->disconnect());
        fakeit::Verify(Method(mockConnectionHandler, purge)).Once();
        BOOST_CHECK(!m_socket->isConnected());
    }

    /**
     * Verify that can start and stop the server repeatedly
     */
    BOOST_FIXTURE_TEST_CASE(StartAndStopServerRepeatedlyTest, TcpServerSocketTest::TestFixture) {
        // Connect once
        BOOST_CHECK(!m_socket->isConnected());
        BOOST_CHECK(m_socket->connect());
        BOOST_CHECK(m_socket->isConnected());
        BOOST_CHECK(m_socket->disconnect());
        fakeit::Verify(Method(mockConnectionHandler, purge)).Once();
        BOOST_CHECK(!m_socket->isConnected());

        // Connect again
        BOOST_CHECK(!m_socket->isConnected());
        BOOST_CHECK(m_socket->connect());
        BOOST_CHECK(m_socket->isConnected());
        BOOST_CHECK(m_socket->disconnect());
        fakeit::Verify(Method(mockConnectionHandler, purge)).Twice();
        BOOST_CHECK(!m_socket->isConnected());
    }

    /**
     * Verify that the server is stopped on destruction
     */
    BOOST_FIXTURE_TEST_CASE(StartAndDestroyTest, TcpServerSocketTest::SetupMocks) {
        {
            cadf::comms::TcpServerSocket socket(TcpServerSocketTest::serverInfo(), &mockConnectionHandler.get(), 10);
            BOOST_CHECK(!socket.isConnected());
            BOOST_CHECK(socket.connect());
            BOOST_CHECK(socket.isConnected());
        }

        fakeit::Verify(Method(mockConnectionHandler, purge)).Once();
    }

    /**
     * Verify that the methods that shouldn't do anything, don't do anything
     */
    BOOST_FIXTURE_TEST_CASE(UnsupportedMethodTest, TcpServerSocketTest::TestFixture) {
        BOOST_CHECK(!m_socket->send(&mockOutputBuffer.get()));
        m_socket->removeMessageListener(&mockListener.get());
    }

    BOOST_AUTO_TEST_SUITE_END()
