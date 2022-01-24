#include "comms/network/handshake/HandshakeResponseMessage.h"
#include "comms/network/serializer/binary/Serializer.h"
#include "comms/network/serializer/json/Serializer.h"

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
     * Determine the size of the data when serialized to JSON
     */
    template<>
    size_t cadf::comms::json::sizeOfJSON<HandshakeResponseDataV1>(const HandshakeResponseDataV1 &data) {
        return numOfCharsKey("clientType") + numOfCharsValue(data.clientType) + numOfCharsKey("clientInstance") + numOfCharsValue(data.clientInstance)
                + 2 // the {} delimiting the JSON structure
                + 1; // the , separating the values
    }

    /*
     * Populate the JSON builder with information contained within the message.
     */
    template<>
    void cadf::comms::json::populateBuilder<HandshakeResponseDataV1>(const HandshakeResponseDataV1 &data, cadf::dom::JSONValue *subRoot,
            cadf::dom::JSONBuilder &builder) {
        builder.addValue(subRoot, "clientType", data.clientType);
        builder.addValue(subRoot, "clientInstance", data.clientInstance);
    }

    /*
     * Load the message data from the provided JSON
     */
    template<>
    void cadf::comms::json::loadFromBuilder<HandshakeResponseDataV1>(HandshakeResponseDataV1 &data, const cadf::dom::JSONValue *subRoot,
            const cadf::dom::JSONExtractor &extractor) {
        data.clientType = extractor.getValue<int>(subRoot, "clientType");
        data.clientInstance = extractor.getValue<int>(subRoot, "clientInstance");
    }
}
