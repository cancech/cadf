#ifndef CAMB_BUS_BUS_H_
#define CAMB_BUS_BUS_H_

#include <map>
#include "comms/bus/BusConnection.h"

namespace cadf::comms {

    /**
     * Interface for a Bus, that allows message passing and communication to take place between different
     * Nodes. The Bus is not expected to perform any message processing of any form, but rather ensuring
     * that the message is propagated from the sender to all receivers.
     */
    class IBus {

        public:
            /**
             * DTOR
             */
            virtual ~IBus() = default;

            /**
             * Called when a new connection is established.
             *
             * @param *connection IBusConnection pointer to the newly established connection
             */
            virtual void connected(IBusConnection *connection) = 0;

            /**
             * Called when a new connection is lost.
             *
             * @param *connection IBusConnection pointer to the connection that was lost
             */
            virtual void disconnected(IBusConnection *connection) = 0;

            /**
             * Send a message from the sender.
             *
             * @param *sender IBusConnection pointer to the connection that is sending the message
             * @param *packet MessagePacket pointer to the packet with the message to send
             */
            virtual void sendMessage(IBusConnection *sender, const MessagePacket *packet) = 0;
    };

    /**
     * Abstract bus that provides the core capabilities a bus is expected to require. Namely a mechanism for routing messages from the sender
     * to the receiver (or potentially multiple receivers). Subclasses are required to provide the means of getting the message to/from a connection,
     * while the AbstractBus provides the means for the internal routing.
     */
    class AbstractBus: public IBus {
        public:
            /**
             * DTOR
             */
            virtual ~AbstractBus() = default;

            /**
             * Process a new connection.
             *
             * @param *connection IBusConnection pointer to the new connection.
             */
            virtual void connected(IBusConnection *connection);

            /**
             * Process the disconnection of an existing connection.
             *
             * @param *connection IBusConnection pointer to the connection that has disconnected.
             */
            virtual void disconnected(IBusConnection *connection);

        protected:
            /**
             * Route the message from the sender to the recipient(s), based on the routing information in the packet.
             *
             * @param *sender IBusConnection pointer to the connection where the message originated.
             * @param *packet MessagePacket containing the message and the relevant routing information
             */
            virtual void routeMessage(IBusConnection *sender, const MessagePacket *packet);

            /**
             * Sends the message to the recipient. The specific bus is to provide the mechanism for sending.
             *
             * @param *sender IBusConnection whom is sending the message
             * @param *recipient IBusConnection to whom to send the message
             * @param *packet MessagePacket that is to be sent
             */
            virtual void sendMessage(IBusConnection *sender, IBusConnection *recipient, const MessagePacket *packet) = 0;

        private:
            /** All connections mapped to their type (outer) and instance (inner) */
            std::map<int, std::multimap<int, IBusConnection*> > m_connections;

            /**
             * Send the message to the exact recipient as prescribed in the packet.
             *
             * @param *sender IBusConnection whom is sending the message
             * @param *packet MessagePacket that is to be sent
             */
            void sendToSingleRecipient(IBusConnection *sender, const MessagePacket *packet);

            /**
             * Broadcast the packet to all available recipients (except the sender).
             *
             * @param *sender IBusConnection whom is sending the message
             * @param *packet MessagePacket that is to be sent
             */
            void broadcastToAll(IBusConnection *sender, const MessagePacket *packet);

            /**
             * Broadcast the packet to all available instances of a given type (except the sender).
             *
             * @param *sender IBusConnection whom is sending the message
             * @param *packet MessagePacket that is to be sent
             */
            void broadcastToAllInstancesSingleType(IBusConnection *sender, const MessagePacket *packet);

            /**
             * Helper that will send the message to all connections in the provided map (except the sender)
             *
             * @param *sender IBusConnection whom is sending the message
             * @param *packet MessagePacket that is to be sent
             * @param &instanceMap std::multimap of all instances to which to broadcast the packet
             */
            void broadcastToAllInstances(IBusConnection *sender, const MessagePacket *packet, std::multimap<int, IBusConnection*> &instanceMap);

            /**
             * Broadcast the packet to all specific instances, regardless of their type
             *
             * @param *sender IBusConnection whom is sending the message
             * @param *packet MessagePacket that is to be sent
             */
            void broadcastToSingleInstanceAllTypes(IBusConnection *sender, const MessagePacket *packet);

            /**
             * Send the message to the specified recipient. If the sender and recipient are not the one and same, it will pass the message
             * to the subclass to ensure delivery to the recipient.
             *
             * @param *sender IBusConnection whom is sending the message
             * @param *recipient IBusConnection whom is to receive the message
             * @param *packet MessagePacket containing the message
             */
            void sendToRecipient(IBusConnection *sender, IBusConnection *recipient, const MessagePacket *packet);
    };
}

#endif /* CAMB_BUS_BUS_H_ */
