#ifndef COMMS_NETWORK_BASICNODECLIENT_H_
#define COMMS_NETWORK_BASICNODECLIENT_H_

#include "comms/node/Node.h"
#include "comms/connection/ClientConnection.h"
#include "comms/network/client/BasicClient.h"
#include "comms/network/socket/TcpClientSocket.h"
#include "comms/network/NetworkInfo.h"

namespace cadf::comms {

    /**
     * Basic client implementation of a Node that is to connect to a remote Bus.
     *
     * @template PROTOCOL the class which defines how messages will be (de)serialized for transmission over the network
     * @template SUPPORTED_MESSAGES... arbitrary list of messages that are to be supported by the server. Each must extend from the base IMessage class.
     */
    template<class PROTOCOL, class ... SUPPORTED_MESSAGES>
    class BasicNodeClient {
        public:

            /**
             * CTOR
             *
             * @param type int to assign to this node
             * @param instance int to assign to this node
             * @param &info const NetworkInfo providing the details of where the server should listen for client connections
             * @param maxDataMsgSize size_t the maximum size for a message to support
             */
            BasicNodeClient(int type, int instance, const NetworkInfo &info, size_t maxMessageSize) : m_msgFactory(512), m_clientSocket(info, maxMessageSize), m_client(&m_clientSocket),
                    m_clientConnection(type, instance, &m_msgFactory, &m_client), m_clientNode(&m_clientConnection) {
                // Register the message specified via the template
                MessageRegistry<PROTOCOL, SUPPORTED_MESSAGES...> msgRegistry;
                msgRegistry.registerMessages(&m_msgFactory);
                registerMessages(&m_msgFactory);
            }

            /**
             * DTOR
             */
            virtual ~BasicNodeClient() = default;

            /**
             * Connect the Node as a client to the server
             *
             * @return bool true if the connection is successfully established (or if already connected)
             */
            virtual bool connect() {
                return m_clientNode.connect();
            }

            /**
             * Disconnect the node from the server
             *
             * @return bool true if the connection is successfully broken (or if already disconnected)
             */
            virtual bool disconnect() {
                return m_clientNode.disconnect();
            }

            /**
             * Check whether or not the node is connected
             *
             * @return bool true if the node is currently connected
             */
            virtual bool isConnected() {
                return m_clientNode.isConnected();
            }

            /**
             * Add a processor to the node, which is to process a specific message when it is received.
             *
             * @param *processor IProcesor to add
             */
            virtual void addProcessor(cadf::comms::IProcessor *processor) {
                m_clientNode.addProcessor(processor);
            }

            /**
             * Send a message, to be routed to the desired destination(s) by the bus
             *
             * @param *msg IMessage to be sent
             * @param type int of the recipient
             * @param instance int of the recipient
             *
             * A cadf::comms::MessageSendingException will be thrown if an issue is encountered attempting to send the message.
             */
            virtual void sendMessage(cadf::comms::IMessage *msg, int type, int instance) {
                m_clientNode.sendMessage(msg, type, instance);
            }

        protected:
            /**
             * Override to register additional message beyond what is specified via the template parameters. Called during server initialization.
             *
             * @param *msgFactory MessageFactory that the server is employing
             */
            void registerMessages(MessageFactory<PROTOCOL> *msgFactory) {
            }

        private:
            //Factory for creating default instances of received messages
            cadf::comms::MessageFactory<PROTOCOL> m_msgFactory;
            // Socket through which to connect to the server
            cadf::comms::TcpClientSocket m_clientSocket;
            // Client for managing the connection to the server
            cadf::comms::BasicClient m_client;
            // Representation of the connection to the server
            cadf::comms::ClientConnection<PROTOCOL> m_clientConnection;
            // The Node for this client
            cadf::comms::Node m_clientNode;
    };
}

#endif /* COMMS_NETWORK_BASICNODECLIENT_H_ */
