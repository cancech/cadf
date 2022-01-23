#define BOOST_TEST_DYN_LINK
#ifdef STAND_ALONE
#   define BOOST_TEST_MODULE Main
#endif
#include <boost/test/unit_test.hpp>

#include "comms/network/socket/TcpClientSocket.h"

namespace TcpClientSocketTest {

    cadf::comms::NetworkInfo serverInfo() {
        cadf::comms::NetworkInfo serverNetInfo;
        serverNetInfo.ipVersion = cadf::comms::NetworkInfo::IPv4;
        serverNetInfo.netAddress = "127.0.0.1";
        serverNetInfo.port = 101010;
        return serverNetInfo;
    }

    struct TestFixture {

        TestFixture() {
            m_socket = new cadf::comms::TcpClientSocket(serverInfo(), 1024);
        }

        ~TestFixture() {
            delete(m_socket);
        }

        cadf::comms::TcpClientSocket *m_socket;
    };
}

/**
 * Test suite for the ServerSocket
 */
BOOST_AUTO_TEST_SUITE(TcpClientSocket_Test_Suite)

/**
 * Verify that can start and stop the server
 */
    BOOST_FIXTURE_TEST_CASE(ConnectToInvalidServerTest, TcpClientSocketTest::TestFixture) {
        BOOST_CHECK(!m_socket->isConnected());
        BOOST_CHECK(!m_socket->connect());
        BOOST_CHECK(!m_socket->isConnected());
        BOOST_CHECK(m_socket->disconnect());
        BOOST_CHECK(!m_socket->isConnected());
    }

    BOOST_AUTO_TEST_SUITE_END()
