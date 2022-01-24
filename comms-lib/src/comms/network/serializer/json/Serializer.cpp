#include "comms/network/serializer/json/Serializer.h"

namespace cadf::comms::json {
    /*
     * CTOR - load the details of the message from the JSON in the buffer
     */
    Deserializer::Deserializer(InputBuffer *buffer): IDeserializer(0, 0, ""), m_root(cadf::dom::JSONParser::parse(buffer->getData())),
            m_extractor(cadf::dom::JSONExtractor(m_root)) {
        m_type = m_extractor.getValue<int>("type");
        m_instance = m_extractor.getValue<int>("instance");
        m_msgType = m_extractor.getValue<std::string>("message");
    }

    /*
     * DTOR
     */
    Deserializer::~Deserializer() {
        delete (m_root);
    }
}
