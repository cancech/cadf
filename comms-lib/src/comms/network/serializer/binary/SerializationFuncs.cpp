#include "comms/network/serializer/binary/SerializationFuncs.h"
#include <string>
#include <vector>

namespace cadf::comms::binary {

    /**
     * Helper that performs the calculation as to how much data is required to serialize a string based on how long it is.
     *
     * @param &size size_t the number of characters in the string
     */
    size_t sizeOfString(const size_t &size) {
        return sizeOfData(size) + sizeof(char) * size;
    }

    /**
     * Calculate the size of a std::string based on the number of characters it contains.
     */
    template<>
    size_t sizeOfData<std::string>(const std::string &data) {
        return sizeOfString(data.length());
    }

    /**
     * Perform the serialization of a string
     */
    template<>
    void serializeData<std::string>(const std::string &data, OutputBuffer *buffer) {
        buffer->append(data.length(), sizeOfData(data.length()));
        for (const char &c : data)
            buffer->append(c, sizeOfData(c));
    }

    /**
     * Perform the deserialization of a string
     */
    template<>
    std::string deserializeData<std::string>(InputBuffer *buffer) {
        size_t strLength = buffer->retrieveNext<size_t>(sizeof(size_t));
        std::string str = "";

        for (size_t i = 0; i < strLength; i++)
            str += buffer->retrieveNext<char>(sizeof(char));

        return str;
    }
}
