#ifndef CAMB_BRIDGENODE_H_
#define CAMB_BRIDGENODE_H_

#include <map>

#include "comms/connection/Connection.h"

namespace cadf::comms {

    /**
     * A specialized node that is to be used to bridge two buses. Two connections are provided, each of which should be connected to a different bus. Any messages that are received on one connection
     * are then forwarded to the other connection based on routing rules that are applied. Since the bridge is a node on the bus, messages must be addressed (or broadcast) to it in order for it to
     * receive them. Once received no processing whatsoever is performed on the message, other than to perform a lookup for whom should be the recipient on the other bus/connection.
     *
     * The routing rules allow the bridge to act as a rely taking any messages address (or broadcast) to the bus, and forward them on to a specific recipient (or broadcast) on the other bus.
     * This allow each bus to send messages, without needing to be aware of the layout of the other bus. The routing rules associate a node type and instance to a message type, meaning that when the
     * bus receives a message of that type, it will be forwarded to the specific node type and instance on the other bus. If no routing is specified, then no message forwarding can be performed.
     */
    class BridgeNode {
        private:
            /**
             * Helper class that performs the routing in one direction of the bridge.
             */
            class MessageForwarder: public IMessageListener {
                    /** The mapping of the rules, message type to the pair<type, instance> */
                    std::map<std::string, std::pair<int, int> > m_routing;
                    /** the connection from which the message will be received */
                    IConnection *m_fromConnection;
                    /** The connection to which the messages will be forwarded */
                    IConnection *m_toConnection;

                    /**
                     * Called when a message has been received on the m_fromConnection.
                     * Note that no memory management is performed, as that is to be handled by the sender
                     *
                     * @param *msg const IMessage pointer to the message received
                     */
                    void messageReceived(const MessagePacket *packet);

                public:
                    /**
                     * CTOR - Creates a new unidirectional forwarding connection
                     *
                     * @param *from IConnection pointer from where the messages are to be received
                     * @param *to IConnection pointer to where the message are to be forwarded.
                     */
                    MessageForwarder(IConnection *from, IConnection *to);

                    /**
                     * DTOR
                     */
                    ~MessageForwarder();

                    /**
                     * Add a rule based on which the forwarding will be performed.
                     *
                     * @param messageType std::string the type of message
                     * @param nodeType int to which message of messageType are to be forwarded (ConnectionConstants::BROADCAST for broadcast)
                     * @param nodeInstance int to which message of messageType are to be forwarded (ConnectionConstants::BROADCAST for broadcast)
                     */
                    void addRule(std::string messageType, int nodeType, int nodeInstance);
            };

        public:
            /**
             * CTOR - creates a bidirectional bridged connection
             *
             * @param *internal IConnection pointer to the "internal" side of the connection
             * @param *external IConnection pointer to the "external" side of the connection
             */
            BridgeNode(IConnection *internal, IConnection *external);

            /**
             * DTOR
             */
            virtual ~BridgeNode() = default;

            /**
             * Add a rule based on which the forwarding will be performed from the External connection to the Internal one.
             *
             * @param messageType std::string the type of message
             * @param nodeType int to which message of messageType are to be forwarded (ConnectionConstants::BROADCAST for broadcast)
             * @param nodeInstance int to which message of messageType are to be forwarded (ConnectionConstants::BROADCAST for broadcast)
             */
            virtual void addForwardToInternalRule(std::string messageType, int nodeType, int nodeInstance);

            /**
             * Add a rule based on which the forwarding will be performed from the Internal connection to the External one.
             *
             * @param messageType std::string the type of message
             * @param nodeType int to which message of messageType are to be forwarded (ConnectionConstants::BROADCAST for broadcast)
             * @param nodeInstance int to which message of messageType are to be forwarded (ConnectionConstants::BROADCAST for broadcast)
             */
            virtual void addForwardToExternalRule(std::string messageType, int nodeType, int nodeInstance);

        private:
            /** The unidirectional forwarding from the Internal to the External connections */
            MessageForwarder m_internalToExternal;
            /** The unidirectional forwarding from the External to the Internal connections */
            MessageForwarder m_externalToInternal;
    };
}

#endif /* CAMB_BRIDGENODE_H_ */
