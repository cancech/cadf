#include "comms/network/socket/TcpServerSocket.h"

#include <functional>
#include <unistd.h>

namespace cadf::comms {

    /*
     * CTOR
     */
    TcpServerSocket::TcpServerSocket(const NetworkInfo &info, IServerConnectionHandler *connectionHandler, int connectionQueueSize) : AbstractTcpSocket(info), m_connectionHandler(connectionHandler),
            m_connectionQueueSize(connectionQueueSize), m_connectThread(this), m_addrlen(sizeof(m_address)) {
    }

    /*
     * DTOR
     */
    TcpServerSocket::~TcpServerSocket() {
        disconnect();
    }

    /*
     * Configure the socket as a server.
     */
    bool TcpServerSocket::establishConnection() {
        int opt = 1;
        if (setsockopt(m_socketFd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
            return false;
        if (bind(m_socketFd, (sockaddr*) &m_address, sizeof(m_address)) < 0)
            return false;
        if (listen(m_socketFd, m_connectionQueueSize) < 0)
            return false;

        m_connectThread.start();
        return true;
    }

    /*
     * Terminate and purge any connected clients
     */
    bool TcpServerSocket::terminateConnection() {
        m_connectThread.stop();
        m_connectionHandler->purge();
        return true;
    }

    /*
     * Cannot send
     */
    bool TcpServerSocket::send(const OutputBuffer *out) {
        return false;
    }

    /*
     * Cannot receive
     */
    void TcpServerSocket::addMessageListener(ISocketMessageReceivedListener *listener) {
        // Intentionally left blank
    }

    /*
     * Cannot receive
     */
    void TcpServerSocket::removeMessageListener(ISocketMessageReceivedListener *listener) {
        // Intentionally left blank
    }

    /*
     * Wait for a client to connect and process it
     */
    void TcpServerSocket::execLoop() {
        int newSock = accept(m_socketFd, (sockaddr*) &m_address, (socklen_t*) &m_addrlen);
        if (newSock > 0)
            m_connectionHandler->handleConnection(newSock);
    }
}
