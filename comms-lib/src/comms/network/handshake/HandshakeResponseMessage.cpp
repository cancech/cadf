#include "comms/network/handshake/HandshakeResponseMessage.h"

#include "comms/network/serializer/dom/BaseSerializer.h"
#include "comms/network/serializer/binary/Serializer.h"

namespace cadf::comms {
    /*
     * Check if the two datas are the same
     */
    bool operator==(const HandshakeResponseDataV1& lhs, const HandshakeResponseDataV1& rhs) {
        return lhs.clientType == rhs.clientType && lhs.clientInstance == rhs.clientInstance;
    }

    /*
     * Stream the data
     */
    std::ostream& operator<<(std::ostream& stream, const HandshakeResponseDataV1& data) {
        stream << "[ clientType = " << data.clientType <<", clientInstance = " << data.clientInstance << " ]";
        return stream;
    }

    /*
     * Determine the size of the data when serialized to binary
     */
    template<>
    size_t cadf::comms::binary::sizeOfData<HandshakeResponseDataV1>(const HandshakeResponseDataV1 &data) {
        return DataSerializer<int>::sizeOf(data.clientType) + DataSerializer<int>::sizeOf(data.clientInstance);
    }

    /*
     * Serialize the data to binary.
     */
    template<>
    void cadf::comms::binary::serializeData<HandshakeResponseDataV1>(const HandshakeResponseDataV1& data, cadf::comms::OutputBuffer *buffer) {
        buffer->append(data.clientType, sizeof(int));
        buffer->append(data.clientInstance, sizeof(int));
    }

    /*
     * Deserialize the data from binary
     */
    template<>
    HandshakeResponseDataV1 cadf::comms::binary::deserializeData<HandshakeResponseDataV1>(cadf::comms::InputBuffer *buffer) {
        HandshakeResponseDataV1 data;
        data.clientType = buffer->retrieveNext<int>(sizeof(int));
        data.clientInstance = buffer->retrieveNext<int>(sizeof(int));
        return data;
    }

    /*
     * Populate the JSON builder with information contained within the message.
     */
    template<>
    cadf::dom::DomNode cadf::comms::dom::buildTree<HandshakeResponseDataV1>(const HandshakeResponseDataV1 &data) {
        cadf::dom::DomNode node = cadf::dom::buildNode("clientType", data.clientType);
        node["clientInstance"] = data.clientInstance;
        return node;
    }

    /*
     * Load the message data from the provided JSON
     */
    template<>
    HandshakeResponseDataV1 cadf::comms::dom::loadFromTree<HandshakeResponseDataV1>(const cadf::dom::DomNode &root) {
        HandshakeResponseDataV1 data;
        data.clientType = root["clientType"];
        data.clientInstance = root["clientInstance"];
        return data;
    }
}
