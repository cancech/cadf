#ifndef CAMB_CONNECTION_CONNECTIONEXCEPTION_H_
#define CAMB_CONNECTION_CONNECTIONEXCEPTION_H_

#include <stdexcept>
#include <string>

namespace cadf::comms {

    /**
     * Exception when trying to use the bus.
     */
    struct BusException: public std::runtime_error {
            BusException(const std::string &reason) : std::runtime_error("Unable to perform Bus Operation: " + reason) {
            }
    };

    /**
     * Exception when trying to send a message
     */
    struct MessageSendingException: public std::runtime_error {
            MessageSendingException(const std::string &type, const std::string &reason) : std::runtime_error("Unable to send message \"" + type + "\": " + reason) {
            }
    };
}

#endif /* CAMB_CONNECTION_CONNECTIONEXCEPTION_H_ */
