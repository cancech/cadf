#include "comms/network/serializer/dom/Serializer.h"

namespace cadf::comms::dom {
    /*
     * CTOR - load the details of the message from the JSON in the buffer
     */
    Deserializer::Deserializer(cadf::dom::DomConverter *converter, InputBuffer *buffer): IDeserializer(0, 0, "") {
        m_root = converter->fromString(buffer->getData());
        m_type = m_root["type"];
        m_instance = m_root["instance"];
        m_msgType = m_root["message"].operator std::string();
    }
}
