#ifndef CAMB_NETWORK_JSON_JSONEXCEPTION_H_
#define CAMB_NETWORK_JSON_JSONEXCEPTION_H_

#include <stdexcept>
#include <string>

namespace cadf::comms::json {

    /**
     * Exception to be used when an issue is encountered parsing a JSON tree
     */
    struct JSONTreeException: public std::runtime_error {
            JSONTreeException(const std::string &error) : std::runtime_error("Error parsing the JSON Tree: " + error) {
            }
    };

    /**
     * Exception to be used when an issue is encountered parsing a JSON string
     */
    struct JSONParseException: public std::runtime_error {
            JSONParseException(const std::string &expected, int index) : std::runtime_error("Error parsing JSON: '" + expected + "' expected at " + std::to_string(index)) {
            }
    };
}

#endif /* CAMB_NETWORK_JSON_JSONEXCEPTION_H_ */
