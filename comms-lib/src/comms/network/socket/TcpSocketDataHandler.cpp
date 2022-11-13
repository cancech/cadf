#include "comms/network/socket/TcpSocketDataHandler.h"
#include "comms/network/socket/SocketException.h"

#include <functional>
#include <algorithm>
#include <sys/socket.h>

namespace cadf::comms {
    /*
     * CTOR
     */
    TcpSocketDataHandler::TcpSocketDataHandler(int socketFd, size_t maxMessageSize) : m_socketFd(socketFd), m_maxMessageSize(maxMessageSize), m_messageBuffer(new char[maxMessageSize]) {
        start();
    }

    /*
     * DTOR
     */
    TcpSocketDataHandler::~TcpSocketDataHandler() {
        stop();
        delete[] (m_messageBuffer);
    }

    /*
     * Start reading messages
     */
    void TcpSocketDataHandler::start() {
        LoopingThread::start();
    }

    /*
     * Stop reading messages
     */
    void TcpSocketDataHandler::stop() {
        LoopingThread::stop();
    }

    /*
     * Add the listener
     */
    void TcpSocketDataHandler::addListener(ISocketMessageReceivedListener *listener) {
        m_listeners.push_back(listener);
    }

    /*
     * Remove the listener
     */
    void TcpSocketDataHandler::removeListener(ISocketMessageReceivedListener *listener) {
        m_listeners.erase(std::remove(m_listeners.begin(), m_listeners.end(), listener), m_listeners.end());
    }

    /*
     * Read data from the socket
     */
    void TcpSocketDataHandler::execLoop() {
        int valread = read(m_socketFd, m_messageBuffer, m_maxMessageSize);
        if (valread <= 0) {
            // TODO error!
        } else {
            // Process the message
            processMessage(valread);
        }
    }

    /*
     * Process the data read from the socket
     */
    void TcpSocketDataHandler::processMessage(size_t messageSize) {
        InputBuffer inBuffer(m_messageBuffer, messageSize);
        for (ISocketMessageReceivedListener *l : m_listeners)
            l->messageReceived(&inBuffer);
    }

    /*
     * Send the message
     */
    void TcpSocketDataHandler::send(const OutputBuffer *out) {
        if (out->getDataSize() > m_maxMessageSize)
            throw SocketException("buffer overflow - data to send is larger than the maximum allowed message size");

        if(::send(m_socketFd, out->getData(), out->getDataSize(), 0) != out->getDataSize())
            throw SocketException("error sending message");
    }
}
