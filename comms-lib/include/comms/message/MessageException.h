#ifndef CAMB_EXCEPTION_MESSAGEEXCEPTION_H_
#define CAMB_EXCEPTION_MESSAGEEXCEPTION_H_

#include <stdexcept>
#include <string>

namespace cadf::comms {

    /**
     * Exception to be used when there is a problem relating to the message type
     */
    struct InvalidMessageTypeException: public std::runtime_error {
            InvalidMessageTypeException(const std::string& type, const std::string& reason) : std::runtime_error("Invalid message type \"" + type + "\": " + reason) {
            }
    };

}

#endif /* CAMB_EXCEPTION_MESSAGEEXCEPTION_H_ */
