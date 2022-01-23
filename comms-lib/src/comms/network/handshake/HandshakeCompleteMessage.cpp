#include "comms/network/handshake/HandshakeCompleteMessage.h"
#include "comms/network/serializer/binary/Serializer.h"
#include "comms/network/serializer/json/Serializer.h"

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
     * Determine the size of the data when serialized to JSON
     */
    template<>
    size_t cadf::comms::json::sizeOfJSON<HandshakeCompleteData>(const HandshakeCompleteData &data) {
        return numOfCharsKey("version") + numOfCharsValue(data.version) + 2; // the {} delimiting the JSON structure
    }

    /*
     * Populate the JSON builder with information contained within the message.
     */
    template<>
    void cadf::comms::json::populateBuilder<HandshakeCompleteData>(const HandshakeCompleteData &data, JSONValue *subRoot, JSONBuilder &builder) {
        builder.addValue(subRoot, "version", data.version);
    }

    /*
     * Load the message data from the provided JSON
     */
    template<>
    void cadf::comms::json::loadFromBuilder<HandshakeCompleteData>(HandshakeCompleteData &data, const JSONValue *subRoot, const JSONExtractor &extractor) {
        data.version = extractor.getValue<unsigned int>(subRoot, "version");
    }
}
