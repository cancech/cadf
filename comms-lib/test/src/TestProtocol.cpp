#include "TestProtocol.h"

std::string MockProtocol::m_expectedDeserializedMessageType = "";

TestSerializer::TestSerializer(std::string msgType, std::string serializedTxt) : ISerializer(msgType), m_serializedText(serializedTxt) {
}

size_t TestSerializer::getSize() const {
    return m_serializedText.size() + 1;
}

void TestSerializer::serialize(cadf::comms::OutputBuffer *buffer) {
    buffer->append(m_serializedText.c_str(), getSize());
}


TestDeserializer::TestDeserializer(std::string expectedType) : IDeserializer(0 ,0, expectedType) {
}


cadf::comms::ISerializer* MockSerializerFactory::buildSerializer(const cadf::comms::IMessage *msg, int type, int instance) const {
    return new TestSerializer(msg->getType(), "SERIALIZED");
}

void MockSerializerFactory::deserializeTo(cadf::comms::IMessage *msg, cadf::comms::IDeserializer *deserializer) const {
}


cadf::comms::IDeserializer* MockProtocol::createDeserializer(cadf::comms::InputBuffer *buffer) {
    return new TestDeserializer(m_expectedDeserializedMessageType);
}
