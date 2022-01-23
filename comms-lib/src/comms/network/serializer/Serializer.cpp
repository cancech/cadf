#include "comms/network/serializer/Serializer.h"

namespace cadf::comms {
    /*
     * CTOR
     */
    ISerializer::ISerializer(const std::string &type) : m_msgType(type) {
    }

    /*
     * CTOR
     */
    IDeserializer::IDeserializer(int type, int instance, std::string msgType) : m_type(type), m_instance(instance), m_msgType(msgType) {
    }

    /*
     * Get the message type
     */
    const std::string& IDeserializer::getMessageType() const {
        return m_msgType;
    }

    /**
     * Get the recipient type.
     */
    int IDeserializer::getRecipientType() const {
        return m_type;
    }

    /**
     * Get the recipient instance.
     */
    int IDeserializer::getRecipientInstance() const {
        return m_instance;
    }
}
