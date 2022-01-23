#ifndef CAMB_NETWORK_SOCKET_ISERVERCONNECTIONHANDLER_H_
#define CAMB_NETWORK_SOCKET_ISERVERCONNECTIONHANDLER_H_

#include "comms/connection/Connection.h"

namespace cadf::comms {

    /**
     * Listener to learn when a new client connects to the server.
     */
    class ITcpServerConnectionListener {
        public:
            /**
             * DTOR
             */
            virtual ~ITcpServerConnectionListener() = default;

            /**
             * Called when a new client is connected.
             *
             * @param *connection IConnection representing the connection to the client
             */
            virtual void clientConnected(IConnection *connection) = 0;

            /**
             * Called when a new client is disconnected.
             *
             * @param *connection IConnection representing the connection to the client
             */
            virtual void clientDisconnected(IConnection *connection) = 0;
    };

    /**
     * Handler that will process clients when they connect to ensure that they are properly integrated.
     */
    class IServerConnectionHandler {
        public:
            /**
             * DTOR
             */
            virtual ~IServerConnectionHandler() = default;

            /**
             * Add a listener to be notified when a client is connected.
             *
             * @param *listener ITcpServerConnectionListener
             */
            virtual void addClientConnectionListener(ITcpServerConnectionListener *listener) = 0;

            /**
             * Remove a listener that is no longer to be notified of new client connections
             *
             * @param *listener ITcpServerConnectionListener
             */
            virtual void removeClientConnectionListener(ITcpServerConnectionListener *listener) = 0;

            /**
             * To be called when a client first establishes a connection. This will process the client
             * and properly integrate it with the server.
             *
             * @param socketFd int the file descriptor of the socket at which the client has been connected
             */
            virtual void handleConnection(int socketFd) = 0;

            /**
             * Purges any connections that were still in the process of connecting.
             */
            virtual void purge() = 0;
    };
}

#endif /* CAMB_NETWORK_SOCKET_ISERVERCONNECTIONHANDLER_H_ */
