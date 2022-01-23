#ifndef CAMB_NETWORK_NETWORKINFO_H_
#define CAMB_NETWORK_NETWORKINFO_H_

#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>

namespace cadf::comms {

    /**
     * Contains the details of a network connection
     */
    struct NetworkInfo {

        /**
         * An enumeration of the different supported Internet Protocols
         */
        enum IPVersion {
            IPv4, IPv6
        };

        /** The version of the protocol for the connection */
        IPVersion ipVersion;
        /** The address of the connection (where to listen as a server and where to connect to as client) */
        std::string netAddress;
        /** The port of the connection (where to listen as a server and where to connect to as client) */
        int port;

        /**
         * Get the STDCPP type indicating the type of connection protocol to use
         *
         * @return int the STDCPP representation
         */
        int typeAsSocketType() const;

        /**
         * Get the STDCPP representation of the version of the connection.
         *
         * @return int the STDCPP representation
         */
        int ipVersionAsAddressFamily() const;

        /**
         * Converts the details of the connection to a socket address.
         *
         * @return sockaddr_in with the address of the socket
         */
        sockaddr_in toSocketAddress() const;
    };
}

#endif /* CAMB_NETWORK_NETWORKINFO_H_ */
