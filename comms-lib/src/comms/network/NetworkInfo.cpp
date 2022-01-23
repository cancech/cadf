#include "comms/network/NetworkInfo.h"
#include <unistd.h>

namespace cadf::comms {
    /*
     * Get the type of socket.
     */
    int NetworkInfo::typeAsSocketType() const {
        return SOCK_STREAM;
    }

    /*
     * Get the version of the IP connection
     */
    int NetworkInfo::ipVersionAsAddressFamily() const {
        switch(ipVersion) {
            case(IPVersion::IPv4):
                return AF_INET;
            case(IPVersion::IPv6):
                return AF_INET6;
        }

        return AF_INET;
    }

    /*
     * Create a socket address from the network information
     */
    sockaddr_in NetworkInfo::toSocketAddress() const {
        sockaddr_in addr;
        addr.sin_family = ipVersionAsAddressFamily();
        addr.sin_port = htons(port);

        if (netAddress.empty())
            addr.sin_addr.s_addr = INADDR_ANY;

        if (inet_pton(ipVersionAsAddressFamily(), netAddress.c_str(), &addr.sin_addr) <= 0) {
            // TODO invalid IP address
        }

        return addr;
    }
}
