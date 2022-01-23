#ifndef CAMB_NETWORK_SERVER_BASICSERVERCONNECTION_H_
#define CAMB_NETWORK_SERVER_BASICSERVERCONNECTION_H_

#include "comms/network/server/ServerConnectionFactory.h"

namespace cadf::comms {

    /**
     * A Basic connection to be used internally within a server to facilitate message passing between the client and the server.
     * This has a limited feature set compared to a "proper" connection, as connection/disconnection and equivalent items are
     * handled separately within the server.
     *
     * @template PROTOCOL the protocol that the server uses to communicate with the client.
     */
    template<class PROTOCOL>
    class BasicServerConnection: public IServerConnection, public AbstractConnection, public ISocketMessageReceivedListener {
        public:

            /**
             * CTOR
             *
             * @param type int of the connection
             * @param instance int of the connection
             * @param *socket ISocketDataHandler through which to pass data back and forth with the client
             * @param *protocolFactory MessageFactory for the specified protocol
             */
            BasicServerConnection(int type, int instance, ISocketDataHandler *socket, MessageFactory<PROTOCOL> *protocolFactory) : AbstractConnection(type, instance, protocolFactory),
                    m_socket(socket), m_protocolFactory(protocolFactory) {
                m_socket->addListener(this);
            }

            /**
             * DTOR
             */
            virtual ~BasicServerConnection() {
                m_socket->removeListener(this);
            }

            /**
             * Get the type of connection, this is used for routing purposes.
             *
             * @return int the type of the connection
             */
            virtual int getType() {
                return AbstractConnection::getType();
            }

            /**
             * Get the instance of the connection, this is used for routing purposes.
             *
             * @return int the instance of the connection type
             */
            virtual int getInstance() {
                return AbstractConnection::getInstance();
            }

            /**
             * Add a listener which is to be notified when a new message is received.
             *
             * @param *listener IMessageListener pointer to the listener to be added.
             */
            virtual void addMessageListener(IMessageListener *listener) {
                AbstractConnection::addMessageListener(listener);
            }

            /**
             * Remove a listener which is to no longer be notified when a new message is received.
             *
             * @param *listener IMessageListener pointer to the listener to be removed.
             */
            virtual void removeMessageListener(IMessageListener *listener) {
                AbstractConnection::removeMessageListener(listener);
            }

            /**
             * The existence of the class instance points to the connection being connected. Will always return true.
             *
             * @return bool true
             */
            virtual bool isConnected() {
                return true;
            }

            /**
             * The existence of the class instance points to the connection being connected, will always return true.
             *
             * @return bool true
             */
            virtual bool connect() {
                return true;
            }

            /**
             * Cannot disconnect from the connection, disconnecting is performed from within the server itself. Will always
             * return false.
             *
             * @return bool false
             */
            virtual bool disconnect() {
                return false;
            }

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
            virtual bool sendMessage(const IMessage *msg, int recipientType, int recipientInstance) {
                MessagePacket packet(msg, recipientType, recipientInstance);
                return sendPacket(&packet);
            }

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
            virtual bool sendPacket(const MessagePacket *packet) {
                std::unique_ptr<OutputBuffer> out(m_protocolFactory->serializeMessage(*packet));
                return m_socket->send(out.get());
            }

            /**
             * Called when a message is received
             *
             * @param *in InputBuffer containing the received message
             */
            virtual void messageReceived(InputBuffer *in) {
                std::unique_ptr<MessagePacket> packet(m_protocolFactory->deserializeMessage(in));
                notifyMessageRecieved(packet.get());
            }

        private:
            /** The socket on which to pass data back and forth with the client */
            ISocketDataHandler *m_socket;
            /** The factory for messages for the given protocol */
            MessageFactory<PROTOCOL> *m_protocolFactory;
    };

    /**
     * Factory that creates Basic IServerConnections
     *
     * @template PROTOCOL indicating the type of protocol that the connections created by the factory are to employ
     */
    template<class PROTOCOL>
    class BasicServerConnectionFactory: public IServerConnectionFactory {
        public:

            /**
             * CTOR
             *
             * @param *protocolFactory MessageFactory for the specified protocol
             */
            BasicServerConnectionFactory(MessageFactory<PROTOCOL> *protocolFactory) : m_protocolFactory(protocolFactory) {
            }

            /**
             * Create a new connection
             *
             * @param type int of the connection
             * @param instance int of the connection
             * @param *socket ISocketDataHandler through which to pass data back and forth with the client
             */
            virtual IServerConnection* createConnection(int type, int instance, ISocketDataHandler *socket) {
                return new BasicServerConnection<PROTOCOL>(type, instance, socket, m_protocolFactory);
            }

        private:
            /** Factory for creating messages within the indicated protocol */
            MessageFactory<PROTOCOL> *m_protocolFactory;
    };

}

#endif /* CAMB_NETWORK_SERVER_BASICSERVERCONNECTION_H_ */
