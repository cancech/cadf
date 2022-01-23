#ifndef CAMB_BUS_BUSCONNECTION_H_
#define CAMB_BUS_BUSCONNECTION_H_

#include "comms/message/MessagePacket.h"


namespace cadf::comms {
    class IBus;

    /**
     * Connection class on the side of the Bus. An IBusConnection is created for any/all received
     * IConnection and represents the Bus side of the connection.
     */
    class IBusConnection {

        public:
            /**
             * DTOR
             */
            virtual ~IBusConnection() = default;

            /**
             * Get the type of the connection.
             *
             * @return in the type of the connection
             */
            virtual int getType() = 0;

            /**
             * Get the instance of the connection type.
             *
             * @return int the instance of the connection type
             */
            virtual int getInstance() = 0;

            /**
             * Disconnect the connection
             *
             * @return bool true if the connection was successfully disconnected.
             *
             * NOTE: the connection is always established from the other end, therefore no connect method
             * is required at the Bus side.
             */
            virtual bool disconnect() = 0;

            /**
             * Send a message across the connection from the IBus to the IConnection at the other end.
             * No memory management need be performed within the IBusConnection, as it is the sender
             * who is responsible for memory management.
             *
             * @param *sender IBusConnection pointer to the connection where the message originated
             * @param *packet MessagePacket pointer to the packet to be sent
             */
            virtual void sendMessage(IBusConnection *sender, const MessagePacket *packet) = 0;

            /**
             * Register the connection with a IBus.
             *
             * @param *bus IBus pointer to the bus with which the connection should be registered
             */
            virtual void registerBus(IBus *bus) = 0;
    };

}

#endif /* CAMB_BUS_BUSCONNECTION_H_ */
