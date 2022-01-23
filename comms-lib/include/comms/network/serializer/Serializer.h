#ifndef CAMB_NETWORK_SERIALIZER_H_
#define CAMB_NETWORK_SERIALIZER_H_

#include <string>
#include "comms/network/Buffer.h"
#include "comms/message/Message.h"

namespace cadf::comms {

    /**
     * Serializer interface through which a protocol provides the logic for serializing a message into network sendable data.
     */
    class ISerializer {

        protected:
            // The type of message being serialized
            std::string m_msgType;

            /**
             * CTOR
             *
             * @param &type const std::string reference to the type of message the serializer will be deserializing
             */
            ISerializer(const std::string &type);

        public:
            /**
             * DTOR
             */
            virtual ~ISerializer() {
            }

            /**
             * Get the size of the serialized message in bytes.
             *
             * @return size_t number of bytes required to store the serialized message
             */
            virtual size_t getSize() const = 0;

            /**
             * Serialize the message into the buffer.
             *
             * @param *buffer OutputBuffer pointer where the serialized message is to be stored
             */
            virtual void serialize(OutputBuffer *buffer) = 0;
    };

    /**
     * Deserializer interface through which a protocol provides the logic for deserializing a message from the network into a message class. Note that
     * the interface doesn't provide any "deserialize" method. The Protocol is expected to create a concrete IDeserializer class instance for the
     * received data, and pass it into ISerializerFactory::deserializeTo to perform the actual deserializaiton. The concrete IDeserializer class is
     * expected to perform the deserialization in a manner that is unique to that protocol.
     */
    class IDeserializer {
        public:
            /**
             * CTOR
             *
             * @param type int the type of recipient
             * @param instance int the instance of the recipient type
             * @param msgType std::string the type of message being deserialized
             */
            IDeserializer(int type, int instance, std::string msgType);

            /**
             * DTOR
             */
            virtual ~IDeserializer() = default;

            /**
             * Get the type of message that the received data describes
             *
             * @return const std::string& reference to the message type
             */
            virtual const std::string& getMessageType() const;

            /**
             * Get the recipient Type the message is intended for.
             *
             * @return int the recipient Type.
             */
            virtual int getRecipientType() const;

            /**
             * Get the recipient Instance the message is intended for.
             *
             * @return int the recipient Instance.
             */
            virtual int getRecipientInstance() const;

        protected:
            // The type who is the intended recipient
            int m_type;
            // The instance who is the intended recipient
            int m_instance;
            // The type of message the deserializer contains
            std::string m_msgType;
    };

    /**
     * Factory interface for the factory that facilitates the (de)serialization process. It creates the concrete serializer for a protocol, and
     * triggers the deserialization once an instance of the received message has been created.
     */
    struct ISerializerFactory {
            /**
             * DTOR
             */
            virtual ~ISerializerFactory() = default;

            /**
             * Build a serializer for the specific message that was provided.
             *
             * @param *msg IMessage pointer to the message which is to be serialized
             * @param type int the type of recipient that is to receive the message
             * @param instance int the instance of the recipient that is to receive the message
             * @return ISerializer* pointer to the concrete serializer. NOTE allocated memory must be managed by the caller.
             */
            virtual ISerializer* buildSerializer(const IMessage *msg, int type, int instance) const = 0;

            /**
             * Perform the deserialization into the provided message from the specified deserializer.
             *
             * @param *msg IMessage into which the received data is to be deserialized
             * @param *deserializer IDeserializer pointer to the deserializer who can perform the deserialization
             */
            virtual void deserializeTo(IMessage *msg, IDeserializer *deserializer) const = 0;
    };
}

#endif /* CAMB_NETWORK_SERIALIZER_H_ */
