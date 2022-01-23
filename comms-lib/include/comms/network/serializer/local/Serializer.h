#ifndef CAMB_NETWORK_LOCAL_SERIALIZER_H_
#define CAMB_NETWORK_LOCAL_SERIALIZER_H_

#include "comms/network/serializer/Serializer.h"

namespace cadf::comms::local {

    /**
     * SerializerFactory for use with a local only protocol. Since a local only protocol doesn't need to serialize for distribution,
     * any attempt to perform serialization or deserialization will throw a ProtocolException.
     */
    struct LocalSerializerFactory: public ISerializerFactory {
            /**
             * DTOR
             */
            virtual ~LocalSerializerFactory() = default;

            /**
             * Throws a ProtocolException when called. Should never be called.
             *
             * @throws ProtocolException when called.
             * @param *msg IMessage is never used
             * @param type int is never used
             * @param instance int is never used
             * @return ISerializer* never returns
             */
            virtual ISerializer* buildSerializer(const IMessage *msg, int type, int instance) const;

            /**
             * Throws a ProtocolException when called. Should never be called.
             *
             * @throws ProtocolException when called.
             * @param *msg IMessage is never used
             * @param *deserializer IDeserializer is never used
             */
            virtual void deserializeTo(IMessage* msg, IDeserializer* deserializer) const;
    };

    /**
     * SerializerFactory for use with a local only protocol. Since a local only protocol doesn't need to serialize for distribution,
     * any attempt to perform serialization or deserialization will throw a ProtocolException.
     */
    class LocalProtocol {
        public:

            /**
             * Creates the SerializerFactory for the protocol. Note that this is purely for compliance the MessageFactory,
             * but any attempt to serialize or deserialize messages will result in a ProtocolException being thrown.
             *
             * @template T data structure containing the data of the message.
             * @param *msg AbstractDataMessage<T> pointer to an example of the type of message the protocol is to "work with"
             * @return ISerializerFactory* pointer to the SerializerFactory to be used for (de)serialization with this protocol.
             */
            template<class T>
            static ISerializerFactory* createSerializerFactory(AbstractDataMessage<T> *msg) {
                return new LocalSerializerFactory();
            }

            /**
             * Throws a ProtocolException when called. Should never be called.
             *
             * @param *buffer InputBuffer is never used
             * @return IDeserializer* never returns
             */
            static IDeserializer* createDeserializer(InputBuffer *buffer);
    };
}

#endif /* CAMB_NETWORK_LOCAL_SERIALIZER_H_ */
