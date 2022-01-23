#ifndef INCLUDE_TESTSERVER_H_
#define INCLUDE_TESTSERVER_H_

#include "comms/bus/LocalBasicBus.h"
#include "comms/network/server/BasicServerConnection.h"
#include "comms/connection/ClientConnection.h"
#include "comms/network/server/ServerBus.h"
#include "comms/network/socket/ServerConnectionHandler.h"
#include "comms/network/handshake/ProtocolHandshake.h"
#include "thread/BasicThreadPool.h"

#include "TestMessage.h"

namespace test {

    template<class PROTOCOL>
    class TestServer {
        public:
            TestServer(const cadf::comms::NetworkInfo &info) : m_msgFactory(), m_connectionFactory(&m_msgFactory),
                    m_handshakeFactory(1024, &m_msgFactory),
                    m_handshakeHandler(&m_handshakeFactory),
                    m_serverConnHandler(&m_handshakeHandler, &m_connectionFactory),
                    m_serverSocket(info, &m_serverConnHandler, 10), m_bus(), m_serverBus(&m_serverSocket, &m_bus) {
                cadf::comms::MessageRegistry<PROTOCOL, TestMessage1, TestMessage2, TestMessage3> msgRegistry;
                msgRegistry.registerMessages(&m_msgFactory);
                m_serverConnHandler.addClientConnectionListener(&m_serverBus);
            }

            bool start() {
                return m_serverSocket.connect();
            }

            bool stop() {
                return m_serverSocket.disconnect();
            }

            bool isUp() {
                return m_serverSocket.isConnected();
            }

        private:
            cadf::comms::MessageFactory<PROTOCOL> m_msgFactory;

            cadf::comms::BasicServerConnectionFactory<PROTOCOL> m_connectionFactory;
            cadf::comms::ProtocolHandshakeFactory<PROTOCOL> m_handshakeFactory;
            cadf::comms::HandshakeHandler m_handshakeHandler;
            cadf::comms::ServerConnectionHandler m_serverConnHandler;
            cadf::comms::TcpServerSocket m_serverSocket;

            cadf::comms::LocalBasicBus m_bus;
            cadf::comms::ServerBus m_serverBus;
    };

}

#endif /* INCLUDE_TESTSERVER_H_ */
