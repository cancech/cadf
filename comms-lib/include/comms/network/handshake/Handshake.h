#ifndef CAMB_NETWORK_HANDSHAKE_HANDSHAKE_H_
#define CAMB_NETWORK_HANDSHAKE_HANDSHAKE_H_

#include "comms/network/socket/TcpSocketDataHandler.h"

namespace cadf::comms {
    /**
     * Listener to be used to notify when the handshake of a connection has been completed.
     */
    class IHandshakeCompleteListener {
        public:
            /**
             * DTOR
             */
            virtual ~IHandshakeCompleteListener() = default;

            /**
             * Called when the handshake is completed for a client connection
             *
             * @param type int of the connection
             * @param instance int of the connection
             * @param socket *ISocketDataHandler through which to pass messages to/from the socket
             */
            virtual void handshakeComplete(int type, int instance, ISocketDataHandler *socket) = 0;
    };

    /**
     * Interface through which to interact with a mechanism for performing a handshake with a connecting client.
     */
    class IHandshake {
        public:
            /**
             * DTOR
             */
            virtual ~IHandshake() = default;

            /**
             * Start the handshake
             */
            virtual void start(IHandshakeCompleteListener *listener) = 0;
    };

    /**
     * Factory through which to create an instance of a class which will process the handshake for a new client connection.
     */
    class IHandshakeFactory {
        public:
            /**
             * DTOR
             */
            virtual ~IHandshakeFactory() = default;

            /**
             * Creates a new instance
             *
             * @param socketFd int the file descriptor of the socket where the client is connected
             * @return IHandshake* for performing the handshake
             */
            virtual IHandshake* create(int socketFd) = 0;
    };
}

#endif /* CAMB_NETWORK_HANDSHAKE_HANDSHAKE_H_ */
