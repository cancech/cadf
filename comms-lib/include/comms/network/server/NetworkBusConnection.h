#ifndef CAMB_NETWORK_SERVER_NETWORKBUSCONNECTION_H_
#define CAMB_NETWORK_SERVER_NETWORKBUSCONNECTION_H_

#include "comms/connection/Connection.h"
#include "comms/bus/BusConnection.h"

namespace cadf::comms {

    /**
     * Specialized IBusConnection that is to be used with network Nodes.
     */
    class NetworkBusConnection: public IBusConnection, public IMessageListener {
        public:
            /**
             * CTOR
             *
             * @param *connection IConnection to the client node
             */
            NetworkBusConnection(IConnection *connection);

            /**
             * DTOR
             */
            virtual ~NetworkBusConnection();

            /**
             * Get the type of the connection.
             *
             * @return in the type of the connection
             */
            virtual int getType();

            /**
             * Get the instance of the connection type.
             *
             * @return int the instance of the connection type
             */
            virtual int getInstance();

            /**
             * Disconnect the connection
             *
             * @return bool true if the connection was successfully disconnected.
             *
             * NOTE: the connection is always established from the other end, therefore no connect method
             * is required at the Bus side.
             */
            virtual bool disconnect();

            /**
             * Send a message across the connection from the IBus to the IConnection at the other end.
             * No memory management need be performed within the IBusConnection, as it is the sender
             * who is responsible for memory management.
             *
             * @param *sender IBusConnection pointer to the connection where the message originated
             * @param *packet MessagePacket pointer to the packet to be sent
             */
            virtual void sendMessage(IBusConnection *sender, const MessagePacket *packet);

            /**
             * Register the connection with a IBus.
             *
             * @param *bus IBus pointer to the bus with which the connection should be registered
             */
            virtual void registerBus(IBus *bus);

        protected:

            /**
             * Called when a message has been received from the client node.
             *
             * @param *packet const MessagePacket that was received and should be forwarded via the bus
             */
            virtual void messageReceived(const MessagePacket *packet);

        private:
            /** The connection to the client node */
            IConnection *m_connection;
            /** The bus that managed packet routing within the server */
            IBus *m_bus;

            /**
             * Helper method that verifies that the bus registration state, throwing an exception when an invalid state
             * was encountered.
             *
             * @param &attemptedAction const std::string indicating what action the connection was attempting to perform
             * @param shouldBeRegistered bool true if the bus is expected to be registered by the caller
             */
            void verifyBusRegistration(const std::string &attemptedAction, bool shouldBeRegistered = true);
    };
}

#endif /* CAMB_NETWORK_SERVER_NETWORKBUSCONNECTION_H_ */
