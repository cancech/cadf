#define BOOST_TEST_DYN_LINK
#ifdef STAND_ALONE
#   define BOOST_TEST_MODULE Main
#endif
#include <boost/test/unit_test.hpp>
#include <fakeit.hpp>

#include "comms/network/socket/ServerConnectionHandler.h"

namespace ServerConnectionHandlerTest {

    /*
     * Helper fixture for initializing and preparing all of the necessary mocks
     */
    struct SetupMocks {

            SetupMocks() {
                fakeit::When(Method(mockHandshakeHandler, performHandshake)).AlwaysReturn();
                fakeit::When(Method(mockConnectionFactory, createConnection)).AlwaysReturn(&mockServerConnection.get());
                fakeit::When(Method(mockListener1, clientConnected)).AlwaysReturn();
                fakeit::When(Method(mockListener2, clientConnected)).AlwaysReturn();
                fakeit::When(Method(mockListener3, clientConnected)).AlwaysReturn();
                fakeit::When(Method(mockListener4, clientConnected)).AlwaysReturn();
            }

            ~SetupMocks() {
                verifyAllMocksChecked();
            }

            void verifyAllMocksChecked() {
                fakeit::VerifyNoOtherInvocations(mockHandshakeHandler, mockSocket, mockConnectionFactory, mockServerConnection, mockListener1, mockListener2, mockListener3, mockListener4);
            }

            fakeit::Mock<cadf::comms::HandshakeHandler> mockHandshakeHandler;
            fakeit::Mock<cadf::comms::ISocketDataHandler> mockSocket;
            fakeit::Mock<cadf::comms::IServerConnectionFactory> mockConnectionFactory;
            fakeit::Mock<cadf::comms::IServerConnection> mockServerConnection;
            fakeit::Mock<cadf::comms::ITcpServerConnectionListener> mockListener1;
            fakeit::Mock<cadf::comms::ITcpServerConnectionListener> mockListener2;
            fakeit::Mock<cadf::comms::ITcpServerConnectionListener> mockListener3;
            fakeit::Mock<cadf::comms::ITcpServerConnectionListener> mockListener4;
    };

    /**
     * Helper fixture to initialize the ServerConnectionHandler instance that is to be used for the tests
     */
    struct TestFixture: public SetupMocks {

            TestFixture() : SetupMocks(), handler(&mockHandshakeHandler.get(), &mockConnectionFactory.get()) {
                handler.addClientConnectionListener(&mockListener1.get());
                handler.addClientConnectionListener(&mockListener2.get());
                handler.addClientConnectionListener(&mockListener3.get());
                handler.addClientConnectionListener(&mockListener4.get());
            }

            ~TestFixture() {
            }

            cadf::comms::ServerConnectionHandler handler;
    };

}

BOOST_AUTO_TEST_SUITE(ServerConnectionHandler_Test_Suite)

/*
 * Verify that a new connection is handshaked.
 */
    BOOST_FIXTURE_TEST_CASE(HandleNewConnectionTest, ServerConnectionHandlerTest::TestFixture) {
        handler.handleConnection(123);
        fakeit::Verify(Method(mockHandshakeHandler, performHandshake).Using(123, &handler)).Once();
    }

    /*
     * Verify that the listeners are notified when the handshake is completed.
     */
    BOOST_FIXTURE_TEST_CASE(HandshakeCompleteWithListenerTest, ServerConnectionHandlerTest::TestFixture) {
        handler.handshakeComplete(1, 2, &mockSocket.get());
        fakeit::Verify(Method(mockConnectionFactory, createConnection).Using(1, 2, &mockSocket.get())).Once();
        fakeit::Verify(Method(mockListener1, clientConnected).Using(&mockServerConnection.get())).Once();
        fakeit::Verify(Method(mockListener2, clientConnected).Using(&mockServerConnection.get())).Once();
        fakeit::Verify(Method(mockListener3, clientConnected).Using(&mockServerConnection.get())).Once();
        fakeit::Verify(Method(mockListener4, clientConnected).Using(&mockServerConnection.get())).Once();
    }

    /*
     * Verify that only the remaining listeners are notified when the handshake is completed.
     */
    BOOST_FIXTURE_TEST_CASE(HandshakeCompleteWithListenerRemovedTest, ServerConnectionHandlerTest::TestFixture) {
        handler.removeClientConnectionListener(&mockListener1.get());
        handler.removeClientConnectionListener(&mockListener4.get());

        handler.handshakeComplete(234, 345, &mockSocket.get());
        fakeit::Verify(Method(mockConnectionFactory, createConnection).Using(234, 345, &mockSocket.get())).Once();
        fakeit::Verify(Method(mockListener2, clientConnected).Using(&mockServerConnection.get())).Once();
        fakeit::Verify(Method(mockListener3, clientConnected).Using(&mockServerConnection.get())).Once();
    }

    BOOST_AUTO_TEST_SUITE_END()
