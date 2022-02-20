#ifndef COMMS_NETWORK_SERIALIZER_DOM_JSONSERIALIZER_H_
#define COMMS_NETWORK_SERIALIZER_DOM_JSONSERIALIZER_H_

#include "comms/network/serializer/dom/BaseSerializer.h"

namespace cadf::comms::dom::json {

    template<class T>
    class JsonSerializer: public Serializer<T> {
        public:
            JsonSerializer(const AbstractDataMessage<T> *msg, int type, int instance):
                Serializer<T>(cadf::dom::json::JsonConverter::instance(), msg, type, instance) {

            }
    };

    class JsonDeserializer: public Deserializer {
        public:
            JsonDeserializer(InputBuffer *buffer): Deserializer(cadf::dom::json::JsonConverter::instance(), buffer) {
            }
    };

    /**
     * SerializerFactory for the (de)serialization of messages to/from JSON.
     *
     * @template T class indicating the type of data that is stored within the message
     */
    template<class T>
    struct JSONSerializerFactory: public TemplateSerializerFactory<JsonSerializer, JsonDeserializer, T> {

            /**
             * CTOR
             */
            JSONSerializerFactory() : TemplateSerializerFactory<JsonSerializer, JsonDeserializer, T>("JSON") {
            }
    };

    /**
     * The protocol through which to handler the (de)serialization of messages to JSON.
     */
    struct JSONProtocol: public Protocol<JSONSerializerFactory, JsonDeserializer> {
    };
}

#endif /* COMMS_NETWORK_SERIALIZER_DOM_JSONSERIALIZER_H_ */
