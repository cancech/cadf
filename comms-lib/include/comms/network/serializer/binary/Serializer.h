#ifndef CAMB_NETWORK_BINARY_SERIALIZER_H_
#define CAMB_NETWORK_BINARY_SERIALIZER_H_

#include "comms/network/serializer/TemplateProtocol.h"
#include "comms/network/NetworkException.h"
#include "comms/network/serializer/binary/SerializationFuncs.h"
#include "comms/message/Message.h"
#include "comms/message/MessageFactory.h"

namespace cadf::comms::binary {
    /**
     * Struct that can be used to allow for uniform calls to serialization and deserialization regardless of the type (pointer or scalar).
     *
     * @template T the type of data that is to be serialized or deserialized.
     */
    template<typename T>
    struct DataSerializer {
            /**
             * Determine the size of scalar data.
             *
             * @param &data const T reference to the scalar data
             */
            static size_t sizeOf(const T &data) {
                return binary::sizeOfData(data);
            }

            /**
             * Serialize scalar data.
             *
             * @param &data const T the data to be serialized.
             * @param *buffer OutputBuffer pointer where the data is to be copied to
             */
            static void serialize(const T &data, OutputBuffer *buffer) {
                binary::serializeData(data, buffer);
            }

            /**
             * Deserialize scalar data.
             *
             * @param *buffer InputBuffer pointer where the data is to be copied from
             *
             * @return T as retrieved from the buffer
             */
            static T deserialize(InputBuffer *buffer) {
                return binary::deserializeData<T>(buffer);
            }
    };

    /**
     * Struct that can be used to allow for uniform calls to serialization and deserialization regardless of the type (pointer or scalar).
     *
     * @template T the type of data that is to be serialized or deserialized.
     */
    template<typename T>
    struct DataSerializer<T*> {
            /**
             * Determine the size of the data the pointer points to
             *
             * @param *data const T pointer to the data
             */
            static size_t sizeOf(const T *data) {
                return binary::sizeOfPointer(data);
            }

            /**
             * Serialize pointer data.
             *
             * @param *data const T the data to be serialized.
             * @param *buffer OutputBuffer pointer where the data is to be copied to
             */
            static void serialize(const T *data, OutputBuffer *buffer) {
                binary::serializePointer(data, buffer);
            }

            /**
             * Deserialize scalar data.
             *
             * @param *buffer InputBuffer pointer where the data is to be copied from
             *
             * @return T* as retrieved from the buffer
             */
            static T* deserialize(InputBuffer *buffer) {
                return binary::deserializePointer<T>(buffer);
            }
    };

    /**
     * Struct that can be used to allow for uniform calls to serialization and deserialization regardless of the type.
     *
     * @template T the type of data that is to be serialized or deserialized.
     * @template numOfElements size_t the number of elements that are present in the array
     */
    template<typename T, size_t numOfElements>
    struct DataSerializer<std::array<T, numOfElements>> {
            /**
             * Determine the size of an array of values
             *
             * @param &data const T reference to the first value of the array
             */
            static size_t sizeOf(const std::array<T, numOfElements> &data) {
                return binary::sizeOfArray(data);
            }

            /**
             * Serialize an array
             *
             * @param &data const std::array reference to the array data
             * @param *buffer OutputBuffer pointer where the data is to be copied to
             */
            static void serialize(const std::array<T, numOfElements> &data, OutputBuffer *buffer) {
                binary::serializeArray(data, buffer);
            }

            /**
             * Deserialize an array
             *
             * @param *buffer InputBuffer pointer where the data is to be copied from
             *
             * @return std::array with the data retrieved from the buffer
             */
            static std::array<T, numOfElements> deserialize(InputBuffer *buffer) {
                return binary::deserializeArray<T, numOfElements>(buffer);
            }
    };

    /**
     * Struct that can be used to allow for uniform calls to serialization and deserialization regardless of the type.
     *
         * @template V the class representing the dynamic array
         * @template T the type that is stored within the array
         * @template Alloc the allocator used with the dynamic array
     */
    template<template<typename, typename> class V, typename T, typename Alloc>
    struct DataSerializer<V<T, Alloc>> {
            /**
             * Determine the size of a dynamic array of values
             *
             * @param *data const reference to the dynamic array
             */
            static size_t sizeOf(const V<T, Alloc> &data) {
                    return binary::sizeOfDynamicArray(data);
            }

            /**
             * Serialize a dynamic array
             *
             * @param &data const reference to the array data
             * @param *buffer OutputBuffer pointer where the data is to be copied to
             */
            static void serialize(const V<T, Alloc> &data, OutputBuffer *buffer) {
                binary::serializeDynamicArray(data, buffer);
            }

            /**
             * Deserialize a dynamic array
             *
             * @param *buffer InputBuffer pointer where the data is to be copied from
             *
             * @return dynamic array with the data retrieved from the buffer
             */
            static V<T, Alloc> deserialize(InputBuffer *buffer) {
                return binary::deserializeDynamicArray<V, T, Alloc>(buffer);
            }
    };

    /**
     * Struct that can be used to allow for uniform calls to serialization and deserialization regardless of the type.
     *
     * @template M the class representing the map
     * @template K the type used for the map key
     * @template T the type that is stored within the map
     * @template Comp the comparison operator for the key
     * @template Alloc the allocator used with the map
     */
    template<template<typename, typename, typename, typename> class M, typename K, typename T, typename Comp, typename Alloc>
    struct DataSerializer<M<K, T, Comp, Alloc>> {
            /**
             * Determine the size of a map of values
             *
             * @param *data const reference to the map
             */
            static size_t sizeOf(const M<K, T, Comp, Alloc> &data) {
                    return binary::sizeOfMap(data);
            }

            /**
             * Serialize a map
             *
             * @param *data const reference to the map
             * @param *buffer OutputBuffer pointer where the data is to be copied to
             */
            static void serialize(const M<K, T, Comp, Alloc> &data, OutputBuffer *buffer) {
                binary::serializeMap(data, buffer);
            }

            /**
             * Deserialize a map
             *
             * @param *buffer InputBuffer pointer where the data is to be copied from
             *
             * @return map with the data retrieved from the buffer
             */
            static M<K, T, Comp, Alloc> deserialize(InputBuffer *buffer) {
                return binary::deserializeMap<M, K, T, Comp, Alloc>(buffer);
            }
    };

    /**
     * Serializer that is responsible for converting an AbstractDataMessage class to a binary representation
     *
     * @template T the class (struct) representing the data that is stored within the message
     */
    template<class T>
    class MessageSerializer: public ISerializer {

        public:
            /**
             * CTOR
             *
             * @param *msg const AbstractDataMessage that is to be serialized
             */
            MessageSerializer(const AbstractDataMessage<T> *msg, int type, int instance): ISerializer(msg->getType()), m_message(msg), m_type(type), m_instance(instance) {
            }

            /**
             * Determine the size of the message when it is serialized to binary.
             *
             * @return size_t the number of bytes required in order to fully serialize the message
             *
             * Note: this is dependent on pre-existing external DataSerializer<T>::sizeOf() functions being available for the population of the data type.
             */
            size_t getSize() const {
                return DataSerializer<int>::sizeOf(m_type) + DataSerializer<int>::sizeOf(m_instance) + DataSerializer<std::string>::sizeOf(m_msgType) + DataSerializer<T>::sizeOf(m_message->getData());
            }

            /**
             * Serialize the data.
             *
             * @param *buffer OutputBuffer pointer where the data is to be copied to
             *
             * Note: this is dependent on pre-existing external DataSerializer<T>::serialize() functions being available for the population of the data type.
             */
            void serialize(OutputBuffer *buffer) {
                DataSerializer<std::string>::serialize(m_msgType, buffer);
                DataSerializer<int>::serialize(m_type, buffer);
                DataSerializer<int>::serialize(m_instance, buffer);
                DataSerializer<T>::serialize(m_message->getData(), buffer);
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
     * Deserializer that is responsible for re-interpreting the binary data to reconstruct the application data from it
     */
    class MessageDeserializer: public IDeserializer {

        public:
            /**
             * CTOR
             *
             * @param *buffer InputBuffer pointer to the buffer where the received binary is stored
             */
            MessageDeserializer(InputBuffer *buffer);

            /**
             * DTOR
             */
            virtual ~MessageDeserializer();

            /**
             * Load the data from the message and populate a data structure with it.
             *
             * @template T the type of class (struct) where the data is contained
             * @return T the data structure with the loaded data
             *
             * Note: this is dependent on pre-existing external DataSerializer<T>::deserialize() functions being available for the population of the data type.
             */
            template<class T>
            T getData() const {
                return DataSerializer<T>::deserialize(m_buffer);
            }

        private:
            // The buffer where the input binary data is stored
            InputBuffer *m_buffer;
    };

    /**
     * SerializerFactory for the (de)serialization of messages to/from Binary.
     *
     * @template T class indicating the type of data that is stored within the message
     */
    template<class T>
    struct BinarySerializerFactory: public TemplateSerializerFactory<MessageSerializer, MessageDeserializer, T> {

            /**
             * CTOR
             */
            BinarySerializerFactory() : TemplateSerializerFactory<MessageSerializer, MessageDeserializer, T>("Binary") {
            }
    };


    /**
     * The protocol through which to handler the (de)serialization of messages to Binary.
     */
    struct BinaryProtocol: public Protocol<BinarySerializerFactory, MessageDeserializer> {};
}

#endif /* CAMB_NETWORK_BINARY_SERIALIZER_H_ */
