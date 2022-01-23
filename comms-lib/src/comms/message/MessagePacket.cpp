#include "comms/message/MessagePacket.h"
#include "comms/Constants.h"

namespace cadf::comms {

    /**
     * CTOR
     */
    MessagePacket::MessagePacket(const IMessage *message, int type, int instance, bool manageMsgMemory) : m_message(message), m_responsibleForMessageMemory(manageMsgMemory), m_type(type), m_instance(instance) {
    }

    /**
     * DTOR
     */
    MessagePacket::~MessagePacket() {
        if (m_responsibleForMessageMemory)
            delete (m_message);
    }

    /**
     * Get the type of recipient
     */
    int MessagePacket::getRecipientType() const {
        return m_type;
    }

    /**
     * Get the instance of the recipient
     */
    int MessagePacket::getRecipientInstance() const {
        return m_instance;
    }

    /**
     * Get the message
     */
    const IMessage* MessagePacket::getMessage() const {
        return m_message;
    }

    /**
     * Check if the type is broadcast
     */
    bool MessagePacket::isTypeBroadcast() const {
        return m_type <= ConnectionConstants::BROADCAST;
    }

    /**
     * Check if the instance is broadcast
     */
    bool MessagePacket::isInstanceBroadcast() const {
        return m_instance <= ConnectionConstants::BROADCAST;
    }

    /**
     * Clone the current packet
     */
    MessagePacket* MessagePacket::clone() const {
        // When cloning, the packet takes on the responsibility of managing the message memory, as it makes a copy of it
        return new MessagePacket(m_message->clone(), m_type, m_instance, true);
    }
}
