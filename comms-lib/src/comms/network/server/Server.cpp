#include "comms/network/server/Server.h"

namespace cadf::comms {

    /*
     * CTOR
     */
    AbstractServer::AbstractServer(ISocket *socket):
            m_serverSocket(socket) {
    }

    /*
     * Start the server socket
     */
    bool AbstractServer::start() {
        return m_serverSocket->connect();
    }

    /*
     * Stop the server socket
     */
    bool AbstractServer::stop() {
        return m_serverSocket->disconnect();
    }

    /*
     * Check whether the server socket is online
     */
    bool AbstractServer::isOnline() {
        return m_serverSocket->isConnected();
    }
}
