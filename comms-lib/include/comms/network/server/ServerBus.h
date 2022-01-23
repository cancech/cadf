#ifndef CAMB_NETWORK_SIMPLESERVER_H_
#define CAMB_NETWORK_SIMPLESERVER_H_

#include <map>

#include "comms/network/server/Server.h"
#include "comms/bus/Bus.h"

namespace cadf::comms {

    /**
     * A server that acts as a Bus for the purpose of message processing and forwarding
     */
    class ServerBus: public AbstractServer {
        public:
            /**
             * CTOR
             *
             * @param *socket ISocket to which clients can connect
             * @param *bus IBus that handles the message distribution
             */
            ServerBus(ISocket *socket, IBus *bus);

            /**
             * DTOR
             */
            virtual ~ServerBus() = default;

            /**
             * On client connection, register the connection with the bus.
             *
             * @param *connection IConnection representing the client who just connected
             */
            virtual void clientConnected(IConnection *connection);

            /**
             * On client disconnection, deregister the connection from the bus.
             *
             * @param *connection IConnection representing the client who just disconnected
             */
            virtual void clientDisconnected(IConnection *connection);

        protected:
            /** The bus which handles the message distribution */
            IBus *m_bus;
            /** Map of all live connections */
            std::map<IConnection*, IBusConnection*> m_busConnections;
    };
}


#endif /* CAMB_NETWORK_SIMPLESERVER_H_ */
