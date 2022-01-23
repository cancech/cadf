#ifndef CAMB_BUS_H_
#define CAMB_BUS_H_

#include <vector>

#include "comms/message/MessageFactory.h"
#include "comms/message/MessageRegistry.h"

namespace cadf::comms {

    /**
     * Listener which is used for notification when a message has been received by a connection.
     */
    class IMessageListener {
        public:
            /**
             * DTOR
             */
            virtual ~IMessageListener() = default;

            /**
             * Called by the connection when a message has been received.
             *
             * @param *msg Message pointer to the message that has been received
             */
            virtual void messageReceived(const MessagePacket *packet) = 0;
    };

    class IConnection {

        public:
            /**
             * DTOR
             */
            virtual ~IConnection() = default;

            /**
             * Get the type of connection, this is used for routing purposes.
             *
             * @return int the type of the connection
             */
            virtual int getType() = 0;

            /**
             * Get the instance of the connection, this is used for routing purposes.
             *
             * @return int the instance of the connection type
             */
            virtual int getInstance() = 0;

            /**
             * Pure virtual method through which to check whether or not the connection is connected at present.
             *
             * @return bool true if it is connected
             */
            virtual bool isConnected() = 0;

            /**
             * Pure virtual method through which the connection is to connect to the appropriate IBus
             * and allow messaging to commence.
             *
             * The concrete connection protocol (subclass) must establish the connection as appropriate
             *
             * @return bool true if the connection was successfully established.
             */
            virtual bool connect() = 0;

            /**
             * Pure virtual method through which the connection to the appropriate IBus is to be terminated.
             *
             * The concrete connection protocol (subclass) must provide the steps for safely terminating the
             * connection.
             *
             * @return bool true if the connection was successfully ended
             */
            virtual bool disconnect() = 0;

            /**
             * Sends a message on the connection. Note that no form of memory management is performed, and the sender is fully
             * responsible for managing any memory allocated to any message that is sent. Upon completion of the send, the client
             * code is able to safely free any allocated memory.
             *
             * Connection::sendMessage(msg);
             * delete(msg);
             *
             * @param *msg const Message pointer to the message that is to be sent
             * @param recipientType int the type of recipient that is to receive the message
             * @param recipientInstance int the instance of the type that is to receive the message
             *
             * @return bool true if the message was successfully sent
             */
            virtual bool sendMessage(const IMessage *msg, int recipientType, int recipientInstance) = 0;

            /**
             * Sends an addressed packet on the connection. Note that no form of memory management is performed, and the sender is fully
             * responsible for managing any memory allocated to any message that is sent. Upon completion of the send, the client
             * code is able to safely free any allocated memory.
             *
             * Connection::sendPacket(packet);
             * delete(packet);
             *
             * @param *packet const MessagePacket pointer to the packet that is to be sent
             *
             * @return bool true if the packet was successfully sent
             */
            virtual bool sendPacket(const MessagePacket *packet) = 0;

            /**
             * Add a listener to be notified when a message is received.
             *
             * @param *listener IMessageListener that is to be added
             */
            virtual void addMessageListener(IMessageListener *listener) = 0;

            /**
             * Remove a message notification listener.
             *
             * @param *listener IMessageListener that is to be removed
             */
            virtual void removeMessageListener(IMessageListener *listener) = 0;
    };

    /**
     * A partial implementation of an IConnection, where the most common features of the connection are implemented for
     * use by all connections, regardless of the specific connection mechanism.
     */
    class AbstractConnection: public IConnection {

        public:
            /**
             * CTOR
             *
             * @template variadic template that lists all of the types of messages that the connection can send and receive
             *
             * @param type int the type of connection this represent
             * @param instance int the instance of the type
             * @param *factory IMessageFactory pointer to the MessageFactory which is to be used for the purpose of creating
             *                 messages. Defaults to a new allocation.
             */
            AbstractConnection(int type, int instance, IMessageFactory *factory);

            /**
             * DTOR
             */
            virtual ~AbstractConnection() = default;

            /**
             * Get the type of connection, this is used for routing purposes.
             *
             * @return int the type of the connection
             */
            virtual int getType();

            /**
             * Get the instance of the connection, this is used for routing purposes.
             *
             * @return int the instance of the connection type
             */
            virtual int getInstance();

            /**
             * Add a listener which is to be notified when a new message is received.
             *
             * @param *listener IMessageListener pointer to the listener to be added.
             */
            virtual void addMessageListener(IMessageListener *listener);

            /**
             * Remove a listener which is to no longer be notified when a new message is received.
             *
             * @param *listener IMessageListener pointer to the listener to be removed.
             */
            virtual void removeMessageListener(IMessageListener *listener);

        protected:
            /** Factory for creating messages */
            IMessageFactory *m_msgFactory;

            /**
             * Notify all registered listeners that a message has been received.
             *
             * @param *msg const Message that was received
             */
            virtual void notifyMessageRecieved(const MessagePacket *packet);

        private:
            /** The type of the connection */
            int m_type;
            /** The instance of the type */
            int m_instance;
            /** Vector of listeners to be notified when a message has been received */
            std::vector<IMessageListener*> m_msgListeners;
    };

}

#endif /* CAMB_BUS_H_ */
