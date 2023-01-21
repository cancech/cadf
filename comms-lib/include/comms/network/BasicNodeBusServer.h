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
     * Basic server implementation for a Bus to which remote nodes can connect to, and through which the nodes can communicate together.
     *
     * @template PROTOCOL the class which defines how messages will be (de)serialized for transmission over the network
     * @template SUPPORTED_MESSAGES... arbitrary list of messages that are to be supported by the server. Each must extend from the base IMessage class.
     */
    template<class PROTOCOL, class ... SUPPORTED_MESSAGES>
    class BasicNodeBusServer {
        public:

            /**
             * CTOR
             *
             * @param *handshakeHandler HandshakeHandler for handshaking with new connections that are made
             * @param *bus IBus which is handle the routing of messages
             * @param &info const NetworkInfo providing the details of where the server should listen for client connections
             * @param maxDataMsgSize size_t the maximum size for a message to support
             */
            BasicNodeBusServer(HandshakeHandler *handshakeHandler, IBus *bus, const NetworkInfo &info, size_t maxDataMsgSize) :
                    m_msgFactory(maxDataMsgSize), m_connectionFactory(&m_msgFactory),
                    m_serverConnHandler(handshakeHandler, &m_connectionFactory),
                    m_serverSocket(info, &m_serverConnHandler, 10), m_bus(bus), m_serverBus(&m_serverSocket, m_bus) {
                // Register the message specified via the template
                MessageRegistry<PROTOCOL, SUPPORTED_MESSAGES...> msgRegistry;
                msgRegistry.registerMessages(&m_msgFactory);
                registerMessages(&m_msgFactory);

                m_serverConnHandler.addClientConnectionListener(&m_serverBus);
            }

            /**
             * DTOR
             */
            virtual ~BasicNodeBusServer() = default;

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
