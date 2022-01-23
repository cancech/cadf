#include "comms/network/serializer/local/Serializer.h"
#include "comms/network/NetworkException.h"

namespace cadf::comms::local {

    /*
     * Throw exception when called
     */
    ISerializer* LocalSerializerFactory::buildSerializer(const IMessage *msg, int type, int instance) const {
        throw ProtocolException("Local", "Serialization not supported. Can only be used for local messaging");
    }

    /*
     * Throw exception when called
     */
    void LocalSerializerFactory::deserializeTo(IMessage *msg, IDeserializer *deserializer) const {
        throw ProtocolException("Local", "Deserialization not supported. Can only be used for local messaging");
    }

    /*
     * Throw exception when called
     */
    IDeserializer* LocalProtocol::createDeserializer(InputBuffer *buffer) {
        throw ProtocolException("Local", "Deserialization not supported. Can only be used for local messaging");
    }
}
