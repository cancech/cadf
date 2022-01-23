#include "comms/network/client/BasicClient.h"
#include <functional>

namespace cadf::comms {
    /*
     * CTOR
     */
    BasicClient::BasicClient(TcpClientSocket *socket) : m_socket(socket), m_messageProcessor(NULL) {
    }

    /*
     * DTOR
     */
    BasicClient::~BasicClient() {
        disconnect();
    }

    /*
     * Set the listener for received messages
     */
    void BasicClient::setListener(ISocketMessageReceivedListener *listener) {
        bool isConnected = m_socket->isConnected();

        if (m_messageProcessor != NULL && isConnected)
            m_socket->removeMessageListener(m_messageProcessor);

        m_messageProcessor = listener;
        if (isConnected)
            m_socket->addMessageListener(m_messageProcessor);
    }

    /*
     * Connect to the server
     */
    bool BasicClient::connect() {
        if (isConnected())
            return true;

        if (m_socket->connect()) {
            if (m_messageProcessor != NULL)
                m_socket->addMessageListener(m_messageProcessor);
            return true;
        }

        return false;
    }

    /*
     * Disconnect from the server
     */
    bool BasicClient::disconnect() {
        if (!isConnected())
            return true;

        if (m_messageProcessor != NULL)
            m_socket->removeMessageListener(m_messageProcessor);
        return m_socket->disconnect();
    }

    /*
     * Check if currently connected
     */
    bool BasicClient::isConnected() {
        return m_socket->isConnected();
    }

    /*
     * Send a message to the server
     */
    bool BasicClient::send(OutputBuffer *out) {
        return m_socket->send(out);
    }
}
