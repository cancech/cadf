#include "comms/network/handshake/HandshakeCompleteMessage.h"

#include "comms/network/serializer/dom/Serializer.h"
#include "comms/network/serializer/binary/Serializer.h"

namespace cadf::comms {
    /*
     * Check if the two datas are the same
     */
    bool operator==(const HandshakeCompleteData &lhs, const HandshakeCompleteData &rhs) {
        return lhs.version == rhs.version;
    }

    /*
     * Stream the data
     */
    std::ostream& operator<<(std::ostream &stream, const HandshakeCompleteData &data) {
        stream << "[ version = " << data.version << " ]";
        return stream;
    }

    /*
     * Determine the size of the data when serialized to binary
     */
    template<>
    size_t cadf::comms::binary::sizeOfData<HandshakeCompleteData>(const HandshakeCompleteData &data) {
        return DataSerializer<unsigned int>::sizeOf(data.version);
    }

    /*
     * Serialize the data to binary.
     */
    template<>
    void cadf::comms::binary::serializeData<HandshakeCompleteData>(const HandshakeCompleteData &data, cadf::comms::OutputBuffer *buffer) {
        buffer->append(data.version, sizeof(unsigned int));
    }

    /*
     * Deserialize the data from binary
     */
    template<>
    HandshakeCompleteData cadf::comms::binary::deserializeData<HandshakeCompleteData>(cadf::comms::InputBuffer *buffer) {
        HandshakeCompleteData data;
        data.version = buffer->retrieveNext<unsigned int>(sizeof(unsigned int));
        return data;
    }

    /*
     * Populate the JSON builder with information contained within the message.
     */
    template<>
    cadf::dom::DomNode cadf::comms::dom::buildTree<HandshakeCompleteData>(const HandshakeCompleteData &data) {
        return cadf::dom::buildNode("version", data.version);
    }

    /*
     * Load the message data from the provided JSON
     */
    template<>
    HandshakeCompleteData cadf::comms::dom::loadFromTree<HandshakeCompleteData>(const cadf::dom::DomNode &root) {
        HandshakeCompleteData data;
        data.version = root["version"];
        return data;
    }
}
