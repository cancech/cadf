#ifndef COMMS_NETWORK_SERIALIZER_DOM_JSONSERIALIZER_H_
#define COMMS_NETWORK_SERIALIZER_DOM_JSONSERIALIZER_H_

#include "comms/network/serializer/dom/BaseSerializer.h"
#include "dom/json/JsonConverter.h"

namespace cadf::comms::dom::json {

    /**
     * Serializes the data contained within a message into a JSON string
     *
     * @template class T the type of data contained within the message
     */
    template<class T>
    class JsonSerializer: public Serializer<T> {
        public:
            /**
             * CTOR
             *
             * @param *msg const AbstractDataMessage containing the data to be serialized
             * @param type int the type of the destination to receive the message
             * @param instance int the instance of the destination to receive the message
             */
            JsonSerializer(const AbstractDataMessage<T> *msg, int type, int instance):
                Serializer<T>(cadf::dom::json::JsonConverter::instance(), msg, type, instance) {
            }
    };

    /**
     * Deserializes the data within the received JSON string into a proper data message
     */
    class JsonDeserializer: public Deserializer {
        public:
            /**
             * CTOR
             *
             * @param *buffer InputBuffer where the data of the received message is stored
             */
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
