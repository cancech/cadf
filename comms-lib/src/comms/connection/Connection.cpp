#include <comms/connection/Connection.h>
#include <algorithm>

namespace cadf::comms {

    /**
     * CTOR
     */
    AbstractConnection::AbstractConnection(int type, int instance, IMessageFactory *factory) : m_msgFactory(factory), m_type(type), m_instance(instance) {
    }

    /**
     * Get the type of the connection
     */
    int AbstractConnection::getType() {
        return m_type;
    }

    /**
     * Get the instance of the connection type
     */
    int AbstractConnection::getInstance() {
        return m_instance;
    }

    /**
     * Add a listener.
     */
    void AbstractConnection::addMessageListener(IMessageListener *listener) {
        m_msgListeners.push_back(listener);
    }

    /**
     * Remove a listener.
     */
    void AbstractConnection::removeMessageListener(IMessageListener *listener) {
        m_msgListeners.erase(std::remove_if(m_msgListeners.begin(), m_msgListeners.end(), [&listener](const IMessageListener *obj) {
            return obj == listener;
        }));
    }

    /**
     * Notify all listeners that a message was received
     */
    void AbstractConnection::notifyMessageRecieved(const MessagePacket *packet) {
        for (IMessageListener *listener : m_msgListeners)
            listener->messageReceived(packet);
    }

}
