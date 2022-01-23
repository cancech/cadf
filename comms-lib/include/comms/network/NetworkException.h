#ifndef CAMB_NETWORK_NETWORKEXCEPTION_H_
#define CAMB_NETWORK_NETWORKEXCEPTION_H_

#include <stdexcept>
#include <string>

namespace cadf::comms {

    /**
     * Exception to be used when the buffer detects an overflow condition
     */
    struct BufferOverflowException: public std::runtime_error {

            /**
             * CTOR
             */
            BufferOverflowException();
    };

    /**
     * Exception to be used when there is an issue processing a message through the protocol (i.e.: serialization/deserialization).
     */
    struct ProtocolException: public std::runtime_error {

            /**
             * CTOR
             *
             * @param &protocolName const std::string the name of the protocol
             * @param &reason const std::string the cause of the error
             */
            ProtocolException(const std::string &protocolName, const std::string &reason);
    };

}

#endif /* CAMB_NETWORK_NETWORKEXCEPTION_H_ */
