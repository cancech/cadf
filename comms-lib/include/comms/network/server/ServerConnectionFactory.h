#ifndef CAMB_NETWORK_SERVER_SERVERCONNECTIONFACTORY_H_
#define CAMB_NETWORK_SERVER_SERVERCONNECTIONFACTORY_H_

#include "comms/connection/Connection.h"
#include "comms/network/Buffer.h"
#include "comms/network/socket/TcpSocketDataHandler.h"

namespace cadf::comms {

    /**
     * Special connection that represents the connection to a client internally within a server.
     */
    class IServerConnection: public IConnection {
        public:
            /**
             * DTOR
             */
            virtual ~IServerConnection() = default;
    };

    /**
     * Factory for creating IServerConnections for the indicated routing and socket.
     */
    class IServerConnectionFactory {
        public:
            /**
             * DTOR
             */
            virtual ~IServerConnectionFactory() = default;

            /**
             * Create a new connection
             *
             * @param type int of the connection
             * @param instance int of the connection
             * @param *socket ISocketDataHandler through which to pass data back and forth with the client
             */
            virtual IServerConnection* createConnection(int type, int instance, ISocketDataHandler *socket) = 0;
    };
}

#endif /* CAMB_NETWORK_SERVER_SERVERCONNECTIONFACTORY_H_ */
