#include "comms/bus/Bus.h"

/**
 * Helper Macro to shorthand the look up and verification of existence of items in a map
 */
#define FIND_IN_MAP(iter, map, key, on_failure) \
    auto iter = map.find(key); \
    if (iter == map.end()) \
        on_failure;

namespace cadf::comms {
    /**
     * Process connection.
     */
    void AbstractBus::connected(IBusConnection *connection) {
        // TODO check if connection is already present to avoid registering multiple times?
        m_connections[connection->getType()].insert({connection->getInstance(), connection});
    }

    /**
     * Process disconnection
     */
    void AbstractBus::disconnected(IBusConnection *connection) {
        std::multimap<int, IBusConnection*> &inner = m_connections[connection->getType()];
        auto typeRange = inner.equal_range(connection->getInstance());
        for (auto i = typeRange.first; i != typeRange.second; i++) {
            if (i->second == connection) {
                inner.erase(i);
                break;
            }
        }
    }

    /**
     * Route the message from the sender to the desired receiver(s).
     */
    void AbstractBus::routeMessage(IBusConnection *sender, const MessagePacket *packet) {
        bool broadcastType = packet->isTypeBroadcast();
        bool broadcastInstance = packet->isInstanceBroadcast();

        if (!broadcastType) {
            // Only want to send to a single instance, which means only a single recipient, or all instances of a type
            if (!broadcastInstance)
                sendToSingleRecipient(sender, packet);
            else
                broadcastToAllInstancesSingleType(sender, packet);
        } else if (!broadcastInstance) {
            // Send to a specific instance of all types
            broadcastToSingleInstanceAllTypes(sender, packet);
        } else {
            // Send to absolutely everyone!
            broadcastToAll(sender, packet);
        }
    }

    /**
     * Send to a single unique recipient, provided it exists
     */
    void AbstractBus::sendToSingleRecipient(IBusConnection *sender, const MessagePacket *packet) {
        FIND_IN_MAP(typeIter, m_connections, packet->getRecipientType(), return);
        auto recipientRange = typeIter->second.equal_range(packet->getRecipientInstance());
        for (auto i = recipientRange.first; i != recipientRange.second; i++)
            sendToRecipient(sender, i->second, packet);
    }

    /**
     * Send to all available connections
     */
    void AbstractBus::broadcastToAll(IBusConnection *sender, const MessagePacket *packet) {
        for (auto typePair : m_connections) {
            broadcastToAllInstances(sender, packet, typePair.second);
        }
    }

    /**
     * Send to all connections of a given type.
     */
    void AbstractBus::broadcastToAllInstancesSingleType(IBusConnection *sender, const MessagePacket *packet) {
        FIND_IN_MAP(typeIter, m_connections, packet->getRecipientType(), return);
        broadcastToAllInstances(sender, packet, typeIter->second);
    }

    /**
     * Helper to send to all connections in the map
     */
    void AbstractBus::broadcastToAllInstances(IBusConnection *sender, const MessagePacket *packet, std::multimap<int, IBusConnection*> &instanceMap) {
        for (auto instancePair : instanceMap)
            sendToRecipient(sender, instancePair.second, packet);
    }

    /**
     * Send to connections of a given instance, regardless of the type
     */
    void AbstractBus::broadcastToSingleInstanceAllTypes(IBusConnection *sender, const MessagePacket *packet) {
        int recipientInstance = packet->getRecipientInstance();
        for (auto typePair : m_connections) {
            FIND_IN_MAP(instanceIter, typePair.second, recipientInstance, continue);
            sendToRecipient(sender, instanceIter->second, packet);
        }
    }

    /**
     * Make sure that the sender and receiver are not the same before sending the message to the recipient.
     */
    void AbstractBus::sendToRecipient(IBusConnection *sender, IBusConnection *recipient, const MessagePacket *packet) {
        if (sender == recipient)
            return;

        sendMessage(sender, recipient, packet);
    }
}
