#ifndef CAMB_NETWORK_JSON_SERIALIZER_H_
#define CAMB_NETWORK_JSON_SERIALIZER_H_

#include "comms/network/serializer/TemplateProtocol.h"
#include "comms/network/serializer/json/SerializationFuncs.h"
#include "comms/network/NetworkException.h"
#include "comms/message/Message.h"
#include "comms/message/MessageFactory.h"

namespace cadf::comms::json {

    /**
     * Serializer that is responsible for converting an AbstractDataMessage class to a JSON representation
     *
     * @template T the class (struct) representing the data that is stored within the message
     */
    template<class T>
    class Serializer: public ISerializer {

        public:
            /**
             * CTOR
             *
             * @param *msg const AbstractDataMessage that is to be serialized
             */
            Serializer(const AbstractDataMessage<T> *msg, int type, int instance) : ISerializer(msg->getType()), m_message(msg), m_type(type), m_instance(instance) {
            }

            /**
             * Determine the size of the message when it is serialized to JSON.
             *
             * @return size_t the number of bytes (characters) required in order to properly serialize the message
             */
            size_t getSize() const {
                // The produced message will look like
                // { "message":"msg->getType()","type":recipientType,"instance":recipientInstance,"data":Serializer::serialize(msg->getData())}
                return m_msgType.size() + json::numOfCharsValue<int>(m_type) + json::numOfCharsValue<int>(m_instance) + json::sizeOfData<T>(m_message->getData()) + 42; // +42 for the extra {"message":"","type":,"instance":,"data":}
            }

            /**
             * Serialize the data.
             *
             * @param *buffer OutputBuffer pointer where the data is to be copied to
             *
             * Note: this is dependent on pre-existing external populateBuilder functions being available for the population of the JSON tree with the data.
             */
            void serialize(OutputBuffer *buffer) {
                cadf::dom::JSONBuilder builder;
                builder.addValue("type", m_type);
                builder.addValue("instance", m_instance);
                builder.addValue("message", m_msgType);
                populateBuilder<T>(m_message->getData(), builder.createNode("data"), builder);
                const cadf::dom::JSONValue *root = builder.getRoot();
                std::string json = root->toString();
                buffer->append(json.c_str(), json.size() + 1); // +1 for the null terminator
                delete (root);
            }

        private:
            // The message that is to be serialized
            const AbstractDataMessage<T> *m_message;
            // The type of recipient
            int m_type;
            // The instance of the type
            int m_instance;
    };

    /**
     * Deserializer that is responsible for re-interpreting JSON to reconstruct the data from it
     */
    class Deserializer: public IDeserializer {

        public:
            /**
             * CTOR
             *
             * @param *buffer InputBuffer pointer to the buffer where the received JSON is stored
             */
            Deserializer(InputBuffer *buffer);

            /**
             * DTOR
             */
            virtual ~Deserializer();

            /**
             * Load the data from the message and populate a data structure with it.
             *
             * @template T the type of class (struct) where the data is contained
             * @return T the data structure with the loaded data
             *
             * Note: this is dependent on pre-existing external loadFromBuilder functions being available for the population of the data type.
             */
            template<class T>
            T getData() const {
                T data;
                loadFromBuilder(data, m_extractor.getChild("data"), m_extractor);
                return data;
            }

        private:
            // The root of the loaded JSON tree created from the JSON string
            const cadf::dom::JSONValue *m_root;
            // The extractor for loading values from the JSON tree
            cadf::dom::JSONExtractor m_extractor;
    };

    /**
     * SerializerFactory for the (de)serialization of messages to/from JSON.
     *
     * @template T class indicating the type of data that is stored within the message
     */
    template<class T>
    struct JSONSerializerFactory: public TemplateSerializerFactory<Serializer, Deserializer, T> {

            /**
             * CTOR
             */
            JSONSerializerFactory() : TemplateSerializerFactory<Serializer, Deserializer, T>("JSON") {
            }
    };

    /**
     * The protocol through which to handler the (de)serialization of messages to JSON.
     */
    struct JSONProtocol: public Protocol<JSONSerializerFactory, Deserializer> {
    };
}

#endif /* CAMB_NETWORK_JSON_SERIALIZER_H_ */
