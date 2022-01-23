#ifndef CAMB_NETWORK_HANDSHAKE_PROTOCOLHANDSHAKE_H_
#define CAMB_NETWORK_HANDSHAKE_PROTOCOLHANDSHAKE_H_

#include "comms/network/socket/TcpSocketDataHandler.h"
#include "comms/network/handshake/Handshake.h"
#include "comms/network/handshake/HandshakeHandler.h"
#include "comms/Constants.h"
#include "comms/message/MessageFactory.h"
#include "comms/message/MessagePacket.h"
#include "comms/network/handshake/HandshakeInitMessage.h"
#include "comms/network/handshake/HandshakeResponseMessage.h"
#include "comms/network/handshake/HandshakeCompleteMessage.h"

namespace cadf::comms {
    /**
     * Handshake for the specified protocol. This will take a very simple approach of:
     *
     * * send message to initialize the handshake (HandshakeInitMessage)
     * * wait for the response, which is expected to include the type/instance of the client (HandshakeResponseMessageV1)
     * * process the client connection with the received information and close out the handshake (HandshakeCompleteMessage)
     */
    template<class PROTOCOL>
    class ProtocolHandshake: public IHandshake, public ISocketMessageReceivedListener {
        public:
            /**
             * CTOR
             *
             * @param *socket ISocketDataHandler for the connected client
             * @param *msgFactory MessageFactory through which to (de)serialize messages to/from the client
             */
            ProtocolHandshake(ISocketDataHandler *socket, MessageFactory<PROTOCOL> *msgFactory) : m_socket(socket), m_listener(NULL), m_msgFactory(msgFactory) {
            }

            /**
             * DTOR
             */
            virtual ~ProtocolHandshake() = default;

            /**
             * Start the handshake
             *
             * @param *listener IHandshakeCompleteListener to notify when the handshake completes
             */
            virtual void start(IHandshakeCompleteListener *listener) {
                m_listener = listener;
                m_socket->addListener(this);

                // Start the handshake
                HandshakeInitMessage msg;
                send(&msg);
            }

            /**
             * Called when a message is received from the client.
             *
             * @param *in InputBuffer with the message received from the client
             */
            virtual void messageReceived(InputBuffer *in) {
                MessagePacket *packet = m_msgFactory->deserializeMessage(in);

                if (packet->getMessage()->getType() == "HandshakeResponseMessageV1") {
                    if (const HandshakeResponseMessageV1 *castMsg = dynamic_cast<const HandshakeResponseMessageV1*>(packet->getMessage()))
                        processHandshakeResponseMessageV1(castMsg->getData());
                }

                delete (packet);
            }

        private:
            /** The handler for the data passing through the socket */
            ISocketDataHandler *m_socket;
            /** Listener to notify when the handshake is complete */
            IHandshakeCompleteListener *m_listener;
            /** Factory for the (de)serialization of messages */
            MessageFactory<PROTOCOL> *m_msgFactory;

            /**
             * Send a message to the client
             */
            void send(IMessage *msg) {
                MessagePacket packet(msg, ConnectionConstants::BROADCAST, ConnectionConstants::BROADCAST);
                std::unique_ptr<OutputBuffer> out(m_msgFactory->serializeMessage(packet));
                if (!m_socket->send(out.get())) {
                    // TODO
                }
            }

            /**
             * Process the message received from the client
             *
             * @param &data const HandshakeResponseDataV1 from the received message
             */
            void processHandshakeResponseMessageV1(const HandshakeResponseDataV1 &data) {
                m_socket->removeListener(this);

                // Complete the handshake
                HandshakeCompleteData responseData = { 1 };
                HandshakeCompleteMessage response(responseData);
                send(&response);

                m_listener->handshakeComplete(data.clientType, data.clientInstance, m_socket);
            }
    };

    /**
     * Factory for creating handshakes dependent on the protocol employed
     */
    template<class PROTOCOL>
    class ProtocolHandshakeFactory: public IHandshakeFactory {
        public:
            /**
             * CTOR
             */
            ProtocolHandshakeFactory(size_t maxMsgSize, MessageFactory<PROTOCOL> *msgFactory) : m_maxMsgSize(maxMsgSize), m_msgFactory(msgFactory) {
            }

            /**
             * DTOR
             */
            virtual ~ProtocolHandshakeFactory() = default;

            /**
             * Create a new handshake
             *
             * @param socketFd int the file descriptor of the socket connected to the client
             * @return IHandshake* for the connection
             */
            IHandshake* create(int socketFd) {
                return new ProtocolHandshake<PROTOCOL>(new TcpSocketDataHandler(socketFd, m_maxMsgSize), m_msgFactory);
            }

        private:
            /** The maximum size of a message that can be sent on the network */
            size_t m_maxMsgSize;
            /** Factory for the (de)serialization of message */
            MessageFactory<PROTOCOL> *m_msgFactory;
    };
}

#endif /* CAMB_NETWORK_HANDSHAKE_PROTOCOLHANDSHAKE_H_ */
