#include "comms/network/server/NetworkBusConnection.h"
#include "comms/connection/ConnectionException.h"
#include "comms/bus/Bus.h"

namespace cadf::comms {
    /*
     * CTOR
     */
    NetworkBusConnection::NetworkBusConnection(IConnection *connection) : m_connection(connection), m_bus(NULL) {
        m_connection->addMessageListener(this);
    }

    /*
     * DTOR
     */
    NetworkBusConnection::~NetworkBusConnection() {
        m_connection->removeMessageListener(this);
        if (m_bus != NULL)
            disconnect();
    }

    /*
     * Get the type of the client connection
     */
    int NetworkBusConnection::getType() {
        return m_connection->getType();
    }

    /*
     * Get the instance of the client connection
     */
    int NetworkBusConnection::getInstance() {
        return m_connection->getInstance();
    }

    /*
     * Disconnect from the bus
     */
    bool NetworkBusConnection::disconnect() {
        verifyBusRegistration("disconnect");

        if (m_connection->disconnect()) {
            m_bus->disconnected(this);
            m_bus = NULL;
            return true;
        }

        return false;
    }

    /*
     * Send a message to the client
     */
    void NetworkBusConnection::sendMessage(IBusConnection *sender, const MessagePacket *packet) {
        m_connection->sendPacket(packet);
    }

    /*
     * Register this connection with the bus
     */
    void NetworkBusConnection::registerBus(IBus *bus) {
        verifyBusRegistration("register bus", false);
        m_bus = bus;
        m_bus->connected(this);
    }

    /*
     * Forward the received message to the bus
     */
    void NetworkBusConnection::messageReceived(const MessagePacket *packet) {
        if (!m_bus)
            return;

        m_bus->sendMessage(this, packet);
    }

    /*
     * Verify the current bus registration state
     */
    void NetworkBusConnection::verifyBusRegistration(const std::string &attemptedAction, bool shouldBeRegistered) {
        if (shouldBeRegistered == (m_bus == NULL))
            throw BusException("Unable to " + attemptedAction + ", " + (m_bus == NULL ? "no " : "") + "bus is registered");
    }
}
