#ifndef INCLUDE_TESTPROTOCOL_H_
#define INCLUDE_TESTPROTOCOL_H_

#include "comms/network/serializer/Serializer.h"
#include <string>


struct TestSerializer: public cadf::comms::ISerializer {
        TestSerializer(std::string msgType, std::string serializedTxt);
        virtual ~TestSerializer() = default;

        virtual size_t getSize() const;
        virtual void serialize(cadf::comms::OutputBuffer *buffer);

        std::string m_serializedText;
};

struct TestDeserializer: public cadf::comms::IDeserializer {
        TestDeserializer(std::string expectedType);
};

struct MockSerializerFactory: public cadf::comms::ISerializerFactory {
        cadf::comms::ISerializer* buildSerializer(const cadf::comms::IMessage *msg, int type, int instance) const;

        void deserializeTo(cadf::comms::IMessage *msg, cadf::comms::IDeserializer *deserializer) const;
};

struct MockProtocol {
        static std::string m_expectedDeserializedMessageType;

        template<class T>
        static cadf::comms::ISerializerFactory* createSerializerFactory(cadf::comms::AbstractDataMessage<T> *msg) {
            return new MockSerializerFactory();
        }

        static cadf::comms::IDeserializer* createDeserializer(cadf::comms::InputBuffer *buffer);
};

#endif /* INCLUDE_TESTPROTOCOL_H_ */
