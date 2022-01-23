#ifndef CAMB_BUS_LOCALBASICBUS_H_
#define CAMB_BUS_LOCALBASICBUS_H_

#include "comms/bus/Bus.h"

namespace cadf::comms {

    /**
     * Basic local bus, which passes the received message from the sending connection
     * to all other available connections. Makes sure that the sender is not among the
     * receivers.
     */
    class LocalBasicBus: public AbstractBus {

        public:
            /**
             * DTOR
             */
            virtual ~LocalBasicBus() = default;

            /**
             * Send a message from an existing connection. The message will be routed to the desired recipient(s).
             *
             * @param *sender IBusConnection pointer to whom sent the message
             * @param *packet MessagePacket pointer to what is being sent
             */
            virtual void sendMessage(IBusConnection *sender, const MessagePacket *packet);

        private:

            /**
             * Sends the message to the recipient, provided that it is also not the sender.
             *
             * @param *sender IBusConnection whom is sending the message
             * @param *recipient IBusConnection to whom to send the message
             * @param *packet MessagePacket that is to be sent
             */
            void sendMessage(IBusConnection *sender, IBusConnection *recipient, const MessagePacket *packet);
    };

}

#endif /* CAMB_BUS_LOCALBASICBUS_H_ */
