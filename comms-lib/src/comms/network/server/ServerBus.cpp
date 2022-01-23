#include "comms/network/server/ServerBus.h"
#include "comms/network/server/NetworkBusConnection.h"
#include <iostream>

namespace cadf::comms {

    /*
     * CTOR
     */
    ServerBus::ServerBus(ISocket *socket, IBus *bus): AbstractServer(socket), m_bus(bus) {
    }

    /*
     * Register the new client with the bus.
     */
    void ServerBus::clientConnected(IConnection *connection) {
        if (m_busConnections.count(connection) != 0)
            return;

        NetworkBusConnection *busConnection = new NetworkBusConnection(connection);
        busConnection->registerBus(m_bus);
        m_busConnections[connection] = busConnection;
    }

    /*
     * Deregister the disconnected client from the bus.
     */
    void ServerBus::clientDisconnected(IConnection *connection) {
        auto iter = m_busConnections.find(connection);
        if (iter == m_busConnections.end())
            return;

        delete(iter->second);
        m_busConnections.erase(iter);
    }
}
