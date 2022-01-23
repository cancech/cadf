#include "comms/network/serializer/binary/Serializer.h"

namespace cadf::comms::binary {

    /*
     * CTOR
     */
    MessageDeserializer::MessageDeserializer(InputBuffer *buffer): IDeserializer(0, 0, ""), m_buffer(buffer) {
        m_msgType = DataSerializer<std::string>::deserialize(m_buffer);
        m_type = DataSerializer<int>::deserialize(buffer);
        m_instance = DataSerializer<int>::deserialize(buffer);
    }

    /*
     * DTOR
     */
    MessageDeserializer::~MessageDeserializer() {
    }
}
