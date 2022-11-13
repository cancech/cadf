#ifndef CAMB_CONNECTION_LOCALCONNECTION_H_
#define CAMB_CONNECTION_LOCALCONNECTION_H_

#include "comms/connection/Connection.h"
#include "comms/bus/Bus.h"
#include "comms/message/MessagePacket.h"

namespace cadf::comms {

    /**
     * Concrete connection implementation that is to be used for local connections. A local connection being defined
     * as a connection that lives within the runtime of a single application and is to be used for iter-application
     * messaging (i.e.: sending messages within a single application).
     */
    class LocalConnection: public AbstractConnection, public IBusConnection {

        public:
            /**
             * CTOR
             *
             * @param *factory IMessageFactory pointer to the factory which is to be responsible for the creation of
             *                 messages
             */
            LocalConnection(int type, int instance, IMessageFactory *factory);

            /**
             * DTOR
             */
            virtual ~LocalConnection() = default;

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
             * Checks whether or not the connection is currently connected.
             *
             * @return bool true if it is
             */
            virtual bool isConnected();

            /**
             * Register the connection with a bus
             *
             * @param *bus IBus to register with the connection
             */
            virtual void registerBus(IBus *bus);

            /**
             * Connect the connection. If already connected, nothing happens.
             *
             * @return bool true if the connection was made successfully.
             */
            virtual bool connect();

            /**
             * Disconnect the connection. If already disconnected, nothing happens.
             *
             * @return bool true if the connection was disconnected.
             */
            virtual bool disconnect();

            /**
             * Sends a message on the connection.
             *
             * For a local connection the IMessage object itself is passed to the receiver, however memory allocated
             * to the message must still be managed on the sender side:
             *
             * LocalConnection::sendMessage(msg);
             * delete(msg);
             *
             * still applied
             *
             * @param *msg const Message pointer to send
             * @param recipientType int the type of recipient that is to receive the message (defaults to broadcast)
             * @param recipientInstance int the instance of the type that is to receive the message (defaults to broadcast)
             *
             * A cadf::comms::MessageSendingException will be thrown if an issue is encountered attempting to send the message.
             */
            virtual void sendMessage(const IMessage *msg, int recipientType = ConnectionConstants::BROADCAST, int recipientInstance = ConnectionConstants::BROADCAST);

            /**
             * Sends an addressed packet on the connection.
             *
             * For a local connection the MessagePacket and the IMessage object contained within are passed to the receiver,
             * however memory allocated to the message must still be managed on the sender side:
             *
             * LocalConnection::sendPacket(packet);
             * delete(packet);
             *
             * still applied
             *
             * @param *packet const MessagePacket pointer to the packet that is to be sent
             *
             * A cadf::comms::MessageSendingException will be thrown if an issue is encountered attempting to send the packet.
             */
            virtual void sendPacket(const MessagePacket *packet);

            /**
             * Sends a message from the Bus
             *
             * @param *sender IBusConnection which is sending the message
             * @param *packet MessagePacket with the message to be sent
             */
            virtual void sendMessage(IBusConnection *sender, const MessagePacket *packet);

        private:
            /** The bus that the connection is registered with */
            IBus *m_bus = NULL;
            /** Flag for whether the connection is currently connected */
            bool m_connected = false;

            /**
             * Verify that the bus is registered (or not). If the registration is in an unwanted state
             * an exception will be thrown. The attempted action specified will be used to derive the
             * message for the exception that is thrown.
             *
             * @param attemptedAction std::string indicating what kind of action was performed
             * @param shouldBeRegistered bool true if should check if the bus has already been registered.
             */
            void verifyBusRegistration(std::string attemptedAction, bool shouldBeRegistered = true);
    };
}

#endif /* CAMB_CONNECTION_LOCALCONNECTION_H_ */
