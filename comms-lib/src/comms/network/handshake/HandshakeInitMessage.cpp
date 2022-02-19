#include "comms/network/handshake/HandshakeInitMessage.h"

#include "comms/network/serializer/dom/Serializer.h"
#include "comms/network/serializer/binary/Serializer.h"

namespace cadf::comms {
    /*
     * Check if the two datas are the same
     */
    bool operator==(const HandshakeInitData& lhs, const HandshakeInitData& rhs) {
        return lhs.maxVersion == rhs.maxVersion;
    }

    /*
     * Stream the data
     */
    std::ostream& operator<<(std::ostream& stream, const HandshakeInitData& data) {
        stream << "[ maxVersion = " << data.maxVersion << " ]";
        return stream;
    }

    /*
     * Determine the size of the data when serialized to binary
     */
    template<>
    size_t cadf::comms::binary::sizeOfData<HandshakeInitData>(const HandshakeInitData &data) {
        return DataSerializer<unsigned int>::sizeOf(data.maxVersion);
    }

    /*
     * Serialize the data to binary.
     */
    template<>
    void cadf::comms::binary::serializeData<HandshakeInitData>(const HandshakeInitData& data, cadf::comms::OutputBuffer *buffer) {
        buffer->append(data.maxVersion, sizeof(unsigned int));
    }

    /*
     * Deserialize the data from binary
     */
    template<>
    HandshakeInitData cadf::comms::binary::deserializeData<HandshakeInitData>(cadf::comms::InputBuffer *buffer) {
        HandshakeInitData data;
        data.maxVersion = buffer->retrieveNext<unsigned int>(sizeof(unsigned int));
        return data;
    }

    /*
     * Populate the JSON builder with information contained within the message.
     */
    template<>
    cadf::dom::DomNode cadf::comms::dom::buildTree<HandshakeInitData>(const HandshakeInitData &data) {
        return cadf::dom::buildNode("maxVersion", data.maxVersion);
    }

    /*
     * Load the message data from the provided JSON
     */
    template<>
    HandshakeInitData cadf::comms::dom::loadFromTree<HandshakeInitData>(const cadf::dom::DomNode &root) {
        HandshakeInitData data;
        data.maxVersion = root["maxVersion"];
        return data;
    }
}
