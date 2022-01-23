#include <comms/node/Node.h>

namespace cadf::comms {

    /**
     * CTOR - connect and add self as a listener to be notified when a message arrives
     */
    Node::Node(IConnection *connection) : m_connection(connection) {
        m_connection->addMessageListener(this);
    }

    /**
     * DTOR - terminate the connection and stop listening for arriving messages
     */
    Node::~Node() {
        m_connection->removeMessageListener(this);
        disconnect();
    }

    /**
     * Attempt connecting
     */
    bool Node::connect() {
        if (isConnected())
            return true;

        return m_connection->connect();
    }

    /**
     * Attempt disconnecting
     */
    bool Node::disconnect() {
        if(!isConnected())
            return true;

        return m_connection->disconnect();
    }

    /**
     * Check if connected
     */
    bool Node::isConnected() {
        return m_connection->isConnected();
    }

    /**
     * Add a new message processor
     */
    void Node::addProcessor(IProcessor *receiver) {
        m_processors[receiver->getType()] = receiver;
    }

    /**
     * Remove an existing message processor
     */
    void Node::removeProcessor(IProcessor *receiver) {
        auto iter = m_processors.find(receiver->getType());
        if (iter == m_processors.end() || iter->second != receiver)
            return;

        m_processors.erase(iter);
    }

    /**
     * Send a message
     */
    bool Node::sendMessage(IMessage *msg, int recipientType, int recipientInstance) {
        if (!isConnected())
            return false;

        return m_connection->sendMessage(msg, recipientType, recipientInstance);
    }

    /**
     * Received messages are directed to the appropriate processor
     */
    void Node::messageReceived(const MessagePacket *packet) {
        auto msg = packet->getMessage();
        auto iter = m_processors.find(msg->getType());
        if (iter == m_processors.end())
            return;

        iter->second->process(msg);
    }

}
