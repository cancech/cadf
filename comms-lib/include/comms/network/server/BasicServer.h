#ifndef COMMS_NETWORK_SERVER_BASICSERVER_H_
#define COMMS_NETWORK_SERVER_BASICSERVER_H_

#include "comms/network/server/BasicServerConnection.h"
#include "comms/connection/ClientConnection.h"
#include "comms/network/server/ServerBus.h"
#include "comms/network/socket/ServerConnectionHandler.h"
#include "comms/network/handshake/ProtocolHandshake.h"
#include "thread/BasicThreadPool.h"

namespace cadf::comms {

    /**
     * Basic server implementation, providing all of the necessary pieces required to start a server bus in a convenient
     * and easy to use package.
     *
     * @template PROTOCOL the class which defines how messages will be (de)serialized for transmission over the network
     * @template SUPPORTED_MESSAGES... arbitrary list of messages that are to be supported by the server. Each must extend from the base IMessage class.
     */
    template<class PROTOCOL, class ... SUPPORTED_MESSAGES>
    class BasicServer {
        public:

            /**
             * CTOR
             *
             * @param &info const NetworkInfo providing the details of where the server should listen for client connections
             * @param maxDataMsgSize size_t the maximum size for a message to support (defaults to AUTO_SIZE, allowing for unlimited message size)
             * @param maxHandshakeMsgSize size_t the maximum size to use for messages employed during handshaking (defaults to 256)
             */
            BasicServer(IBus *bus, const NetworkInfo &info, size_t maxDataMsgSize = MessageConstants::AUTO_SIZE, size_t maxHandshakeMsgSize = 256) :
                    m_msgFactory(maxDataMsgSize), m_connectionFactory(&m_msgFactory),
                    m_handshakeFactory(maxHandshakeMsgSize, &m_msgFactory),
                    m_handshakeHandler(&m_handshakeFactory),
                    m_serverConnHandler(&m_handshakeHandler, &m_connectionFactory),
                    m_serverSocket(info, &m_serverConnHandler, 10), m_bus(bus), m_serverBus(&m_serverSocket, m_bus) {
                // Register the message specified via the template
                MessageRegistry<PROTOCOL, SUPPORTED_MESSAGES...> msgRegistry;
                msgRegistry.registerMessages(&m_msgFactory);
                registerMessages(&m_msgFactory);

                m_serverConnHandler.addClientConnectionListener(&m_serverBus);
            }

            /**
             * Start the server
             *
             * @return bool true if the server was started successfully (or already started)
             */
            bool start() {
                return m_serverSocket.connect();
            }

            /**
             * Stop the server
             *
             * @return bool true if the server was stopped successfully (or already stopped)
             */
            bool stop() {
                return m_serverSocket.disconnect();
            }

            /**
             * Check if the server is started
             *
             * @return bool true if the server is started
             */
            bool isUp() {
                return m_serverSocket.isConnected();
            }

        protected:
            /**
             * Override to register additional message beyond what is specified via the template parameters. Called during server initialization.
             *
             * @param *msgFactory MessageFactory that the server is employing
             */
            void registerMessages(MessageFactory<PROTOCOL> *msgFactory) {
            }

        private:
            //Factory for creating default instances of received messages
            MessageFactory<PROTOCOL> m_msgFactory;

            // Creates internal connections for clients when they connect
            BasicServerConnectionFactory<PROTOCOL> m_connectionFactory;
            // Creates the handshake mechanism for a connected client
            ProtocolHandshakeFactory<PROTOCOL> m_handshakeFactory;
            // Performs the handshake for each new client
            HandshakeHandler m_handshakeHandler;
            // Handles the client connections to the server
            ServerConnectionHandler m_serverConnHandler;
            // The socket on which to listen for new clients
            TcpServerSocket m_serverSocket;

            // The bus on which to perform message passing
            IBus *m_bus;
            // Performs message passing between the internal bus and external clients
            ServerBus m_serverBus;
    };
}

#endif /* COMMS_NETWORK_SERVER_BASICSERVER_H_ */
