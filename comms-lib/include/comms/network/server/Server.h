#ifndef CAMB_NETWORK_SERVER_H_
#define CAMB_NETWORK_SERVER_H_

#include "comms/connection/Connection.h"
#include "comms/network/socket/ISocket.h"
#include "comms/network/socket/IServerConnectionHandler.h"

namespace cadf::comms {

    /**
     * Interface for a server, through which it can be managed. Including start/stop and processing client connections.
     */
    class IServer: public ITcpServerConnectionListener {
        public:
            /**
             * DTOR
             */
            virtual ~IServer() = default;

            /**
             * Starts the server
             *
             * @return bool true if the server was started successfully
             */
            virtual bool start() = 0;

            /**
             * Stops the server
             *
             * @return bool true if the server was stopped successfully
             */
            virtual bool stop() = 0;

            /**
             * Check whether the server is online
             *
             * @return bool true if the server is online
             */
            virtual bool isOnline() = 0;
    };

    /**
     * An abstract server that provides logic for the managing of the server socket, but the extending class must provide the necessary logic
     * to manage client connections.
     */
    class AbstractServer: public IServer {
        public:
            /**
             * CTOR
             *
             * @param *socket ISocket to which clients can connect
             */
            AbstractServer(ISocket *socket);

            /**
             * DTOR
             */
            virtual ~AbstractServer() = default;

            /**
             * Starts the server
             *
             * @return bool true if the server was started successfully
             */
            virtual bool start();

            /**
             * Stops the server
             *
             * @return bool true if the server was stopped successfully
             */
            virtual bool stop();

            /**
             * Check whether the server is online
             *
             * @return bool true if the server is online
             */
            virtual bool isOnline();

        private:
            /** The socket to which clients can connect */
            ISocket *m_serverSocket;
    };
}

#endif /* CAMB_NETWORK_SERVER_H_ */
