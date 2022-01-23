#include "comms/network/serializer/json/SerializationFuncs.h"

namespace cadf::comms::json {
    /*
     * Strings are the characters themselves, surrounded by quotes ("string").
     */
    template<>
    size_t numOfCharsValue<std::string>(const std::string &val) {
        return val.size() + 2;
    }

    /*
     * Chars are represented as ints in the JSON.
     */
    template<>
    size_t numOfCharsValue<char>(const char &val) {
        return numOfCharsValue<int>(val);
    }

    /*
     * Keys are surrounded by quotes followed by colon ("key":)
     */
    size_t numOfCharsKey(const std::string &key) {
        return key.size() + 3;
    }
}
