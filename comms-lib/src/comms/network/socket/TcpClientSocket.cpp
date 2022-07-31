#include "comms/network/socket/TcpClientSocket.h"

namespace cadf::comms {

    /*
     * CTOR
     */
    TcpClientSocket::TcpClientSocket(const NetworkInfo &info, size_t maxMessageSize) : AbstractTcpSocket(info), m_maxMessageSize(maxMessageSize), m_dataSocket(NULL) {
    }

    /*
     * DTOR
     */
    TcpClientSocket::~TcpClientSocket() {
        disconnect();
    }

    /*
     * Connect to the server
     */
    bool TcpClientSocket::establishConnection() {
        if (::connect(m_socketFd, (sockaddr*) &m_address, sizeof(m_address)) < 0)
            return false;

        m_dataSocket = new TcpSocketDataHandler(m_socketFd, m_maxMessageSize);
        for (ISocketMessageReceivedListener *l: m_listeners)
            m_dataSocket->addListener(l);
        return true;
    }

    /*
     * Disconnect from the server
     */
    bool TcpClientSocket::terminateConnection() {
        if (m_dataSocket) {
            delete (m_dataSocket);
            m_dataSocket = NULL;
        }

        return true;
    }

    /*
     * Send a message
     */
    bool TcpClientSocket::send(const OutputBuffer *out) {
        if (!isConnected() || out->getDataSize() > m_maxMessageSize)
            return false;

        return m_dataSocket->send(out);
    }

    /*
     * Add a listener for received messages
     */
    void TcpClientSocket::addMessageListener(ISocketMessageReceivedListener *listener) {
        m_listeners.insert(listener);
        if (m_dataSocket)
            m_dataSocket->addListener(listener);
    }

    /*
     * Remove a listener for received messages
     */
    void TcpClientSocket::removeMessageListener(ISocketMessageReceivedListener *listener) {
        m_listeners.erase(listener);
        if (m_dataSocket)
            m_dataSocket->removeListener(listener);
    }
}
