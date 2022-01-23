#ifndef CAMB_NETWORK_TEMPLATEPROTOCOL_H_
#define CAMB_NETWORK_TEMPLATEPROTOCOL_H_

#include "comms/network/serializer/Serializer.h"

namespace cadf::comms {

    /**
     * Reusable ISerializerFactory that uses templates to specify which type of serializer and deserializer classes to create/use.
     *
     * @template SERIALIZER<T> the type of class to use for the serializer
     * @template DESERIALIZER<T> the type of class to use for the deserializer
     * @template T the type of message class the serializer and deserializer is to work with
     */
    template<template<class T> class SERIALIZER, class DESERIALIZER, class T>
    class TemplateSerializerFactory: public ISerializerFactory {

        public:
            /**
             * CTOR
             *
             * @param protocolName std::string the name of the protocol (used for exception/error reporting)
             */
            TemplateSerializerFactory(std::string protocolName): m_protocolName(protocolName) {
            }

            /**
             * DTOR
             */
            virtual ~TemplateSerializerFactory() = default;

            /**
             * Build the serializer for the specified message.
             *
             * @throws ProtocolException if an unsupported message is to be serialized
             * @param *msg const IMessage pointer to the concrete message to be serialized
             * @param type int the type of recipient that is to receive the message
             * @param instance int the instance of the recipient that is to receive the message
             * @return ISerializer* pointer to the concrete serializer for the message. NOTE: allocated memory must be managed by the caller
             */
            ISerializer* buildSerializer(const IMessage *msg, int type, int instance) const {
                const AbstractDataMessage<T> *concreteMsg = dynamic_cast<const AbstractDataMessage<T>*>(msg);
                if (!concreteMsg)
                    throw ProtocolException(m_protocolName, "Invalid message type - Only data messages are supported");

                return new SERIALIZER<T>(concreteMsg, type, instance);
            }

            /**
             * Perform the deserialization, populating the message with the data contained within the deserializer.
             *
             * @throws ProtocolException if an unsupported message is to be deserialized, or an invalid deserializer is provided
             * @param *msg IMessage pointer to the message into which the data is to be deserialized
             * @param *deserializer IDeserializer which will perform the deserialization
             */
            void deserializeTo(IMessage *msg, IDeserializer *deserializer) const {
                AbstractDataMessage<T> *concreteMsg = dynamic_cast<AbstractDataMessage<T>*>(msg);
                if (!concreteMsg)
                    throw ProtocolException(m_protocolName, "Invalid message type - Only data messages are supported");

                DESERIALIZER *concreteDeserializer = dynamic_cast<DESERIALIZER*>(deserializer);
                if (!concreteDeserializer)
                    throw ProtocolException(m_protocolName, "Invalid deserializer provided");

                concreteMsg->setData(concreteDeserializer->template getData<T>());
            }

        private:
            // The name of the protocol
            std::string m_protocolName;
    };

    /**
     * Prototype Protocol that can be used to shorthand the process of creating protocol.
     *
     * @template SERIALIZER_FACTORY<class T> the type of class to use for the ISerializerFactory. Must contain a constructor that matches the signature of SERIALIZER_FACTORY<T>()
     * @template DESERIALIZER the type of class to use for the IDeserializer. Must contain a constructor that matches the signature of DESERIALIZER(InputBuffer*)
     */
    template<template<class T> class SERIALIZER_FACTORY, class DESERIALIZER>
    struct Protocol {

            /**
             * Creates the ISerializerFactory for use with the defined protocol. The ISerializerFactory will be created for the type of message that is passed in.
             *
             * @template T class indicating the data that is stored within the message
             * @param *msg AbstractDataMessage<T> dummy instance of the type of message that the SerializerFactory is to be associated with
             * @return ISerializerFactory* pointer to the factory that will create serializers for messages of the specified type. NOTE: allocated memory must be managed by the caller.
             */
            template<class T>
            static ISerializerFactory* createSerializerFactory(AbstractDataMessage<T> *msg) {
                return new SERIALIZER_FACTORY<T>();
            }

            /**
             * Creates the IDeserializer for this protocol that will deserialize a message from the data contained within the buffer.
             *
             * @param *buffer InputBuffer containing the data of the message
             * @return IDeserializer* pointer to the deserializer responsible for loading the details of the message. NOTE: allocated memory must be managed by the caller.
             */
            static IDeserializer* createDeserializer(InputBuffer *buffer) {
                return new DESERIALIZER(buffer);
            }
    };
}

#endif /* CAMB_NETWORK_TEMPLATEPROTOCOL_H_ */
