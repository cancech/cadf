#ifndef CAMB_CONNECTION_CLIENTCONNECTION_H_
#define CAMB_CONNECTION_CLIENTCONNECTION_H_

#include "comms/Constants.h"
#include "comms/connection/Connection.h"

#include "comms/network/client/Client.h"
#include "comms/network/Buffer.h"
#include "comms/network/socket/ISocketMessageReceivedListener.h"
#include "comms/network/handshake/HandshakeInitMessage.h"
#include "comms/network/handshake/HandshakeResponseMessage.h"
#include "comms/network/handshake/HandshakeCompleteMessage.h"

namespace cadf::comms {

    /**
     * The connection a client is to use when connecting to a remote bus over the network.
     *
     * @template PROTOCOL the class representing the messaging protocol employed
     */
    template<class PROTOCOL>
    class ClientConnection: public AbstractConnection, public ISocketMessageReceivedListener {

        public:

            /**
             * CTOR
             *
             * @param type int of the connection
             * @param instance int of the connection
             * @param *factory MessageFactory for creating new instances of messages for the specified protocol
             * @param *client IClient throw which to communicate with the bus over the network
             */
            ClientConnection(int type, int instance, MessageFactory<PROTOCOL> *factory, IClient *client) : AbstractConnection(type, instance, factory), m_msgFactory(factory), m_client(client) {
                client->setListener(this);
            }

            /**
             * Checks whether or not the connection is currently connected.
             *
             * @return bool true if it is
             */
            virtual bool isConnected() {
                return m_client->isConnected();
            }

            /**
             * Connect the connection. If already connected, nothing happens.
             *
             * @return bool true if the connection was made successfully.
             */
            virtual bool connect() {
                return m_client->connect();
            }

            /**
             * Disconnect the connection. If already disconnected, nothing happens.
             *
             * @return bool true if the connection was disconnected.
             */
            virtual bool disconnect() {
                return m_client->disconnect();
            }

            /**
             * Attempts to send a message to the bus for distribution.
             *
             * @param *msg const IMessage to send
             * @param recipientType int indication which type of recipient should receive the message (defaults to BROADCAST)
             * @param recipientInstance int indication which instance of recipient should receive the message (defaults to BROADCAST)
             * @return bool true if the message was successfully sent
             */
            virtual bool sendMessage(const IMessage *msg, int recipientType = ConnectionConstants::BROADCAST, int recipientInstance = ConnectionConstants::BROADCAST) {
                if (!isConnected())
                    return false;

                MessagePacket packet(msg, recipientType, recipientInstance);
                return sendPacket(&packet);
            }

            /**
             * Attempts to send the specified package to the bus for distribution.
             *
             * @param *packet const MessagePacket to send
             * @return bool true if the message was successfully sent
             */
            virtual bool sendPacket(const MessagePacket *packet) {
                if (!isConnected())
                    return false;

                std::unique_ptr<OutputBuffer> out(m_msgFactory->serializeMessage(*packet));
                return m_client->send(out.get());
            }

            /**
             * Processes a message that was received from the bus.
             *
             * @param *in InputBuffer containing the message
             */
            virtual void messageReceived(InputBuffer *in) {
                std::unique_ptr<MessagePacket> packet(m_msgFactory->deserializeMessage(in));

                // TODO the handshaking on the client end should really be handled somewhere else...
                if (packet->getMessage()->getType() == "HandshakeInitMessage") {
                    HandshakeResponseDataV1 responseData = { getType(), getInstance() };
                    HandshakeResponseMessageV1 response(responseData);
                    if (!sendMessage(&response)) {
                        // TODO should report failure to send the message in some manner
                    }
                } else {
                    notifyMessageRecieved(packet.get());
                }
            }

        private:
            /** The factory that creates instances of messages that are received */
            MessageFactory<PROTOCOL> *m_msgFactory;
            /** The client with which to communicate with the bus */
            IClient *m_client;
    };

}

#endif /* CAMB_CONNECTION_CLIENTCONNECTION_H_ */
