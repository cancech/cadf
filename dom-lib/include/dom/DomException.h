#ifndef DOM_DOMEXCEPTION_H_
#define DOM_DOMEXCEPTION_H_

#include <stdexcept>
#include <string>

namespace cadf::dom {

    /**
     * Exception to be used when an issue is encountered parsing a JSON string
     */
    struct ParseException: public std::runtime_error {

            /**
             * CTOR
             *
             * @param &expected const std::string indication of which was expected (such as specific character or type)
             * @param index int where within the parsed string the value was expected
             */
            ParseException(const std::string &expected, int index) : std::runtime_error("Error parsing: '" + expected + "' expected at " + std::to_string(index)) {
            }
    };
}

#endif /* DOM_DOMEXCEPTION_H_ */
