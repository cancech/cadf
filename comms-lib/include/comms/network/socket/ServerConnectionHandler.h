#ifndef CAMB_NETWORK_SOCKET_SERVERCONNECTIONHANDLER_H_
#define CAMB_NETWORK_SOCKET_SERVERCONNECTIONHANDLER_H_

#include "comms/network/handshake/HandshakeHandler.h"
#include "comms/network/server/ServerConnectionFactory.h"
#include "comms/network/server/Server.h"
#include "comms/network/socket/IServerConnectionHandler.h"
#include "comms/network/socket/TcpServerSocket.h"
#include "comms/Constants.h"

#include <vector>

namespace cadf::comms {

    /**
     * Handler for client connections that are made to the server. Ensures that the client connections are fully and properly processed prior to joining the overall system.
     */
    class ServerConnectionHandler: public IServerConnectionHandler, public IHandshakeCompleteListener {

        public:
            /**
             * CTOR
             *
             * @param *handshakeHandler HandshakeHandler to establish proper communications
             * @param *connFactory IServerConnectionFactory for creating the connections that are used internally
             */
            ServerConnectionHandler(HandshakeHandler *handshakeHandler, IServerConnectionFactory *connFactory);

            /**
             * DTOR
             */
            virtual ~ServerConnectionHandler();

            /**
             * Add a listener for when a new client is connected
             *
             * @param *listener ITcpServerConnectionListener
             */
            virtual void addClientConnectionListener(ITcpServerConnectionListener *listener);

            /**
             * Remove a listener that should cease to be notified of a new connection
             *
             * @param *listener ITcpServerConnectionListener
             */
            virtual void removeClientConnectionListener(ITcpServerConnectionListener *listener);

            /**
             * Handle a new client connection, start the handshake process.
             *
             * @param socketFd int the file descriptor of the socket at which the client has connected
             */
            virtual void handleConnection(int socketFd);

            /**
             * To be called upon the completion of the handshake with a newly connected client. Establishes the new client as a new connection with the the system.
             *
             * @param type int the of the connected client
             * @param instance int of the connected client
             * @param *socket ISocketDataHandler overwhich the connections passes data
             */
            virtual void handshakeComplete(int type, int instance, ISocketDataHandler *socket);

            /**
             * Purge all waiting connections.
             */
            virtual void purge();

        private:
            /** Listeners waiting for a new client connection to be established */
            std::vector<ITcpServerConnectionListener*> m_connectionListeners;
            /** Handles the handshaking process of new client connections */
            HandshakeHandler *m_handshakeHandler;
            /** Creates internal connections for newly connected clients */
            IServerConnectionFactory *m_serverConnectionFactory;
    };
}

#endif /* CAMB_NETWORK_SOCKET_SERVERCONNECTIONHANDLER_H_ */
