#include <comms/node/BridgeNode.h>

namespace cadf::comms {

    /**
     * CTOR - prepare for message forwarding
     */
    BridgeNode::MessageForwarder::MessageForwarder(IConnection *from, IConnection *to): m_fromConnection(from), m_toConnection(to) {
        m_fromConnection->addMessageListener(this);
        m_fromConnection->connect();
    }

    /**
     * DTOR
     */
    BridgeNode::MessageForwarder::~MessageForwarder() {
        m_fromConnection->removeMessageListener(this);
        m_fromConnection->disconnect();
    }

    /**
     * Add a forwarding rule
     */
    void BridgeNode::MessageForwarder::addRule(std::string messageType, int nodeType, int nodeInstance) {
        m_routing[messageType] = std::pair<int,int>(nodeType, nodeInstance);
    }

    /**
     * When message is received, forward it if a rule has been established
     */
    void BridgeNode::MessageForwarder::messageReceived(const MessagePacket *packet) {
        auto msg = packet->getMessage();
        auto iter = m_routing.find(msg->getType());

        // Make sure that there is a route configured for this message
        if (iter == m_routing.end())
            return;

        m_toConnection->sendMessage(msg, iter->second.first, iter->second.second);
    }



    /**
     * CTOR - create the forwarders for each direction
     */
    BridgeNode::BridgeNode(IConnection *internal, IConnection *external) : m_internalToExternal(internal, external), m_externalToInternal(external, internal) {
    }

    /**
     * Add External to Internal forwarding rule
     */
    void BridgeNode::addForwardToInternalRule(std::string messageType, int nodeType, int nodeInstance) {
        m_externalToInternal.addRule(messageType, nodeType, nodeInstance);
    }

    /**
     * Add Internal to External forwarding rule
     */
    void BridgeNode::addForwardToExternalRule(std::string messageType, int nodeType, int nodeInstance) {
        m_internalToExternal.addRule(messageType, nodeType, nodeInstance);
    }
}
