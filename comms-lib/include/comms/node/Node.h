#ifndef CAMB_NODE_H_
#define CAMB_NODE_H_

#include <comms/connection/Connection.h>
#include <comms/message/Message.h>
#include <comms/node/Processor.h>
#include <map>

namespace cadf::comms {

    /**
     * The Node is the main mechanism for interfacing with the network. It will connected to the created bus
     * via the specified connection on creation, and then disconnect when destroyed. The node is considered to be
     * an end-point where processing is to take place. To process a message on receipt a message processor (IProcessor)
     * must be registered, and only messages with a registered processor can be processed.
     *
     * No checks are performed on the sending of a message, however the IConnection provided must be able to send
     * the message. Requirements for this are dependent on the connection, with simplest requirement being registering
     * the appropriate message with the MessageRegistry.
     */
    class Node: public IMessageListener {

        public:
            /**
             * CTOR
             *
             * @param *connection IConnection pointer to the connection the node is to use for reaching the Bus.
             */
            Node(IConnection *connection);

            /**
             * DTOR
             * On destruction it automatically terminates the connection to the bus.
             */
            virtual ~Node();

            /**
             * Attempt connecting to the bus.
             *
             * @return bool true if the connection was successfully made
             */
            virtual bool connect();

            /**
             * Attempt disconnecting from the bus.
             *
             * @return bool true if the connection was successfully terminated
             */
            virtual bool disconnect();

            /**
             * Check whether the node is connected to the bus
             *
             * @return bool true if the node is currently connected
             */
            virtual bool isConnected();

            /**
             * Register an IProcessor with the node, for the purpose of processing messages.
             *
             * @param *processor IProcessor pointer to the processor for a single message type
             */
            virtual void addProcessor(IProcessor *processor);

            /**
             * Remove an IProcessor from the node, to stop it from receiving and processing messages
             *
             * @param *processor IProcessor pointer to the processor for a single message type
             */
            virtual void removeProcessor(IProcessor *processor);

            /**
             * Send a message across the connection to the bus.
             *
             * @param *msg IMessage pointer to the message to send
             * @param recipientType int the type of recipient for the message
             * @param recipientInstance int the instance of the type of recipient
             *
             * @return bool true if the message was sent successfully
             */
            virtual bool sendMessage(IMessage *msg, int recipientType, int recipientInstance);

        protected:
            /**
             * Called when the node receives a message. The message is directed to the appropriate processor for processing.
             *
             * @param *msg const IMessage pointer to the message that has been received.
             */
            virtual void messageReceived(const MessagePacket *packet);

        private:
            /** The connection to the bus */
            IConnection *m_connection;
            /** Map of all registered processor, key is the type of message and the value is the processor itself */
            std::map<std::string, IProcessor*> m_processors;
    };

}

#endif /* CAMB_NODE_H_ */
