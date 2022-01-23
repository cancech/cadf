#include "comms/network/NetworkException.h"

namespace cadf::comms {
    /*
     * CTOR
     */
    BufferOverflowException::BufferOverflowException() : std::runtime_error("Buffer overflow detected") {
    }

    /*
     * CTOR
     */
    ProtocolException::ProtocolException(const std::string &protocolName, const std::string &reason) : std::runtime_error("Error processing the protocol " + protocolName + ": " + reason) {
    }
}
