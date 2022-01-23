#include "comms/network/socket/AbstractTcpSocket.h"
#include <unistd.h>

namespace cadf::comms {
    /**
     * CTOR
     */
    AbstractTcpSocket::AbstractTcpSocket(const NetworkInfo &info): m_socketFd(-1) {
        m_netProtocol = info.typeAsSocketType();
        m_address = info.toSocketAddress();
    }

    /**
     * DTOR
     */
    AbstractTcpSocket::~AbstractTcpSocket() {
        disconnect();
    }

    /*
     * Create the socket
     */
    bool AbstractTcpSocket::createSocket() {
        m_socketFd = socket(m_address.sin_family, m_netProtocol, PF_UNSPEC);
        return isConnected();
    }

    /*
     * Close the socket
     */
    bool AbstractTcpSocket::closeSocket() {
        if (close(m_socketFd) < 0)
            return false;

        m_socketFd = -1;
        return true;
    }

    /**
     * A valid socket is a sign that it is connected
     */
    bool AbstractTcpSocket::isConnected() {
        return m_socketFd >= 0;
    }

    /**
     * Create the socket and connect it
     */
    bool AbstractTcpSocket::connect() {
        if (isConnected())
            return true;

        if (createSocket()) {
            if (establishConnection())
                return true;
            else
                closeSocket();
        }
        
        return false;
    }

    /**
     * Disconnect the socket
     */
    bool AbstractTcpSocket::disconnect() {
        if (!isConnected())
            return true;

        if (closeSocket())
            return terminateConnection();

        return false;
    }
}
