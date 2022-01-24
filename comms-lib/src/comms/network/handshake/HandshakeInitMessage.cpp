#include "comms/network/handshake/HandshakeInitMessage.h"
#include "comms/network/serializer/binary/Serializer.h"
#include "comms/network/serializer/json/Serializer.h"

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
     * Determine the size of the data when serialized to JSON
     */
    template<>
    size_t cadf::comms::json::sizeOfJSON<HandshakeInitData>(const HandshakeInitData &data) {
        return numOfCharsKey("maxVersion") + numOfCharsValue(data.maxVersion) + 2; // the {} delimiting the JSON structure
    }

    /*
     * Populate the JSON builder with information contained within the message.
     */
    template<>
    void cadf::comms::json::populateBuilder<HandshakeInitData>(const HandshakeInitData &data, cadf::dom::JSONValue *subRoot,
            cadf::dom::JSONBuilder &builder) {
        builder.addValue(subRoot, "maxVersion", data.maxVersion);
    }

    /*
     * Load the message data from the provided JSON
     */
    template<>
    void cadf::comms::json::loadFromBuilder<HandshakeInitData>(HandshakeInitData &data, const cadf::dom::JSONValue *subRoot,
            const cadf::dom::JSONExtractor &extractor) {
        data.maxVersion = extractor.getValue<unsigned int>(subRoot, "maxVersion");
    }
}
