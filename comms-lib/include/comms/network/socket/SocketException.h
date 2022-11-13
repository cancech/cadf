#ifndef COMMS_NETWORK_SOCKET_SOCKETEXCEPTION_H_
#define COMMS_NETWORK_SOCKET_SOCKETEXCEPTION_H_

#include <stdexcept>
#include <string>

namespace cadf::comms {

    /**
     * Exception when trying to use a socket.
     */
    struct SocketException: public std::runtime_error {
            SocketException(const std::string &reason) : std::runtime_error(reason) {
            }
    };
}

#endif /* COMMS_NETWORK_SOCKET_SOCKETEXCEPTION_H_ */
