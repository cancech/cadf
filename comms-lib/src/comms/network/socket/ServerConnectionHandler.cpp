#include "comms/network/socket/ServerConnectionHandler.h"

#include <algorithm>

namespace cadf::comms {
    /*
     * CTOR
     */
    ServerConnectionHandler::ServerConnectionHandler(HandshakeHandler *handshakeHandler, IServerConnectionFactory *connFactory) : m_handshakeHandler(handshakeHandler),
            m_serverConnectionFactory(connFactory) {
    }

    /*
     * DTOR
     */
    ServerConnectionHandler::~ServerConnectionHandler() {
        purge();
    }

    /*
     * Add a listener
     */
    void ServerConnectionHandler::addClientConnectionListener(ITcpServerConnectionListener *listener) {
        m_connectionListeners.push_back(listener);
    }

    /*
     * Remove a listener
     */
    void ServerConnectionHandler::removeClientConnectionListener(ITcpServerConnectionListener *listener) {
        m_connectionListeners.erase(std::remove_if(m_connectionListeners.begin(), m_connectionListeners.end(), [&listener](const ITcpServerConnectionListener *obj) {
            return obj == listener;
        }));
    }

    /*
     * Start the handshake process.
     */
    void ServerConnectionHandler::handleConnection(int socketFd) {
        m_handshakeHandler->performHandshake(socketFd, this);
    }

    /*
     * Upon the completion of a handshake, make the connection available within the system.
     */
    void ServerConnectionHandler::handshakeComplete(int type, int instance, ISocketDataHandler *socket) {
        IServerConnection *connection = m_serverConnectionFactory->createConnection(type, instance, socket);
        for (ITcpServerConnectionListener *l : m_connectionListeners)
            l->clientConnected(connection);
    }

    /*
     * TBD?
     */
    void ServerConnectionHandler::purge() {
        // TODO (TBD?)
    }
}
