#ifndef CAMB_NETWORK_JSON_SERIALIZATIONFUNCS_H_
#define CAMB_NETWORK_JSON_SERIALIZATIONFUNCS_H_

#include <sstream>

#include "comms/network/Buffer.h"
#include "comms/network/serializer/json/JSONBuilder.h"
#include "comms/network/serializer/json/JSONExtractor.h"
#include "comms/network/serializer/json/JSONParser.h"

namespace cadf::comms::json {

    template<class T>
    struct Serializer;

    /**
     * Implementation of this function is to be provided with each data (type T) that is to be used with the JSON Protocol.
     *
     * This is to determine the exact number of characters that the generated JSON string contains (not including null terminator).
     *
     * @template T the type of class (struct) representing the data to send
     * @param &data const T reference to the data that is to be converted to JSON.
     * @return size_t the size of the generated JSON text.
     */
    template<class T>
    size_t sizeOfJSON(const T &data);

    /**
     * Determine the number of characters in the generated JSON string including the null terminatlr
     *
     * @template T the type of class (struct) representing the data to send
     * @param &data const T reference to the data that is to be converted to JSON.
     * @return size_t the size of the generated JSON text.
     */
    template<class T>
    size_t sizeOfData(const T &data) {
        return sizeOfJSON(data) + 1;
    }

    /**
     * Implementation of this function is to be provided with each data (type T) that is to be used with the JSON Protocol.
     *
     * Populates the JSONBuilder with the contents of the data.
     *
     * @template T the type of class (struct) representing the data to send
     * @param &data const T reference to the data that is to be converted to JSON.
     * @param &builder JSONBuilder reference to the builder for creating the JSON structure
     */
    template<class T>
    void populateBuilder(const T &data, JSONValue *subRoot, JSONBuilder &builder);

    /**
     * Implementation of this function is to be provided with each data (type T) that is to be used with the JSON Protocol.
     *
     * Load the contents of the data from the JSON tree.
     *
     * @throws std::out_of_range or cadf::comms::json::JSONTreeException or cadf::comms::json::JSONParseException if there is an issue parsing the data from the JSON in the buffer
     * @template T the type of class (struct) representing the data to load
     * @param &data T reference to the data whose contents is to be loaded
     * @param &extractor JSONExtractor reference to the extractor for loading specific values from the tree
     */
    template<class T>
    void loadFromBuilder(T &data, const JSONValue *subRoot, const JSONExtractor &extractor);

    /**
     * Determine the number of characters required to represent the value in the JSON string. By default it will simply convert the value to a string and provide the number of
     * characters, more specialized overrides can be provided to perform more specific conversions (such as string and char).
     *
     * @template T the type of the value
     * @param &val const T reference to the value
     * @return size_t the number of characters required to represent the value in JSON
     */
    template<typename T>
    size_t numOfCharsValue(const T &val) {
        std::stringstream ss;
        ss << val;
        return ss.str().size();
    }

    template<>
    size_t numOfCharsValue<std::string>(const std::string &val);
    template<>
    size_t numOfCharsValue<char>(const char &val);

    /**
     * Determine the number of characters required to represent a key in the JSON string.
     *
     * @param &key const std::string
     * @return size_t the number of characters required to represent the key
     */
    size_t numOfCharsKey(const std::string &key);
}

#endif /* CAMB_NETWORK_JSON_SERIALIZATIONFUNCS_H_ */
