#include "comms/connection/LocalConnection.h"
#include "comms/connection/ConnectionException.h"

namespace cadf::comms {

    /**
     * CTOR
     */
    LocalConnection::LocalConnection(int type, int instance, IMessageFactory *factory) : AbstractConnection(type, instance, factory) {
    }

    int LocalConnection::getType() {
        return AbstractConnection::getType();
    }

    int LocalConnection::getInstance() {
        return AbstractConnection::getInstance();
    }

    /**
     * Check whether currently connected.
     */
    bool LocalConnection::isConnected() {
        return m_connected;
    }

    /**
     * Register the bus with the connection.
     */
    void LocalConnection::registerBus(IBus *bus) {
        verifyBusRegistration("register bus", false);

        m_bus = bus;
    }

    /**
     * Verify that the bus is registered
     */
    void LocalConnection::verifyBusRegistration(std::string attemptedAction, bool shouldBeRegistered) {
        if (shouldBeRegistered == (m_bus==NULL))
            throw BusException("Unable to " + attemptedAction + (!shouldBeRegistered ? " no" : "") + " bus is registered");
    }

    /**
     * Connect to the bus.
     */
    bool LocalConnection::connect() {
        verifyBusRegistration("connect");

        if (isConnected())
            return true;

        m_bus->connected(this);
        m_connected = true;
        return true;
    }

    /**
     * Disconnect from the bus.
     */
    bool LocalConnection::disconnect() {
        verifyBusRegistration("disconnect");

        if (!isConnected())
            return true;

        m_bus->disconnected(this);
        m_connected = false;
        return true;
    }

    /**
     * Send a message to the bus.
     */
    bool LocalConnection::sendMessage(const IMessage *msg, int recipientType, int recipientInstance) {
        MessagePacket packet(msg, recipientType, recipientInstance);
        return sendPacket(&packet);
    }

    bool LocalConnection::sendPacket(const MessagePacket *packet) {
        verifyBusRegistration("send message");

        if (!isConnected())
            return false;
        if (!m_msgFactory->isMessageRegistered(packet->getMessage()->getType()))
            throw MessageSendingException(packet->getMessage()->getType(), "message type has not been registered with the MessageFactory");

        m_bus->sendMessage(this, packet);
        return true;
    }

    /**
     * Send a message from the bus.
     */
    void LocalConnection::sendMessage(IBusConnection *connection, const MessagePacket *packet) {
        const IMessage* msg = packet->getMessage();
        if (m_msgFactory->isMessageRegistered(msg->getType()))
            notifyMessageRecieved(packet);
    }
}
