#ifndef CAMB_MESSAGE_MESSAGEPACKET_H_
#define CAMB_MESSAGE_MESSAGEPACKET_H_

#include "comms/message/Message.h"

namespace cadf::comms {

    /**
     * Packet for an IMessage that wraps the message in routing information.
     *
     * The routing information is managed as a combination of two values:
     *
     * - Recipient Type
     * - Recipient Instance
     *
     * The goal is the be able to differentiate the different types of recipients via the type (i.e.: NodeProvider vs NodeConsumer)
     * and the instance to distinguish between multiple copies of the same type of recipient (i.e.: primary vs secondary). Using
     * the combination of values any type and specific instance of it can be addressed directly. A negative value in either field
     * will be treated as a sign to broadcast to all possible connected recipients for that field. For example:
     *
     * - type=-1,  instance=-1 >>> route to all connected recipients
     * - type=123, instance=-1 >>> route to all connected instance of type 123
     * - type=-1, instance=123 >>> route to the 123 instances of all connected types
     */
    class MessagePacket {
        public:
            /**
             * CTOR
             *
             * Creates a new packet to route the message. A negative recipient or instance will be treated as a broadcast
             * for that field.
             *
             * @param *message const IMessage that being sent
             * @param type int the type of recipient to which it should be routed
             * @param instance int which specific instance of the recipient type that the message should be routed to
             * @param manageMsgMemory bool flag for whether or not the MessagePacket is to take ownership of the message memory (defaults to false)
             */
            MessagePacket(const IMessage *message, int type, int instance, bool manageMsgMemory = false);

            /**
             * DTOR
             */
            virtual ~MessagePacket();

            /**
             * Get the type of recipient that it should be routed to
             *
             * @return int the type of recipient
             */
            virtual int getRecipientType() const;

            /**
             * Get the instance of the type of recipient to which the message should be routed
             *
             * @return int the instance of the recipient
             */
            virtual int getRecipientInstance() const;

            /**
             * Get the message that is being sent.
             *
             * @return IMessage* pointer to the message being sent.
             */
            virtual const IMessage* getMessage() const;

            /**
             * Check if the message is to be broadcast to all types.
             *
             * @return bool true if the message should be broadcast to all types
             */
            virtual bool isTypeBroadcast() const;

            /**
             * Check if the message is to be broadcast to all instances.
             *
             * @return bool true is the message should be broadcast to all instances
             */
            virtual bool isInstanceBroadcast() const;

            /**
             * Create a clone of the current packet, with allocated memory that must be managed by the caller.
             *
             * @return MessagePacket* pointer to the cloned packet.
             */
            virtual MessagePacket* clone() const;

        private:
            /** The message being sent */
            const IMessage *m_message;
            /** Flag for whether or not the packet is responsible for cleaning up the memory allocated to the message */
            bool m_responsibleForMessageMemory;
            /** The type of the recipient */
            int m_type;
            /** The instance of the recipient */
            int m_instance;
    };
}

#endif /* CAMB_MESSAGE_MESSAGEPACKET_H_ */
