#ifndef CAMB_NETWORK_SOCKET_TCP_ABSTRACTTCPSOCKET_H_
#define CAMB_NETWORK_SOCKET_TCP_ABSTRACTTCPSOCKET_H_

#include "comms/network/socket/ISocket.h"

namespace cadf::comms {

    /**
     * Abstract class containing the common element required for a TCP Socket.
     */
    class AbstractTcpSocket: public ISocket {
        public:
            /**
             * CTOR
             *
             * @param &info const NetworkInfo indicating the network details of where/how to create the socket
             */
            AbstractTcpSocket(const NetworkInfo &info);

            /**
             * DTOR
             */
            virtual ~AbstractTcpSocket();

            /**
             * Connect the socket
             *
             * @return bool true if the socket is successfully connected
             */
            virtual bool connect();

            /**
             * Disconnect the socket
             *
             * @return bool true if the socket is successfully disconnected
             */
            virtual bool disconnect();

            /**
             * Check if the socket is connected.
             *
             * @return bool true if the socket is currently connected
             */
            virtual bool isConnected();

        protected:
            /** The file descriptor of the socket (-1 if invalid) */
            int m_socketFd;
            /** The address of the socket */
            sockaddr_in m_address;
            /** The type of protocol the socket is to use */
            int m_netProtocol;

            /**
             * Called after creating the socket to allow for the concrete method of connecting.
             *
             * @return bool true if the connection is successfully established
             */
            virtual bool establishConnection() = 0;

            /**
             * Called after the socket was successfully closed to complete the connection termination.
             *
             * @return bool true if the connection was successfully terminated
             */
            virtual bool terminateConnection() = 0;

        private:
            /**
             * Creates a new socket. Note: does not check if the socket already exists (repeated calls without closing in between will fail)
             *
             * @return true if the socket was successfully created.
             */
            bool createSocket();

            /**
             * Closes the socket. Note: does not check if the socket exists prior to closing it (calling when the socket is not open will result in failure).
             *
             * @return true if the socket was successfully closed.
             */
            bool closeSocket();
    };
}

#endif /* CAMB_NETWORK_SOCKET_TCP_ABSTRACTTCPSOCKET_H_ */
