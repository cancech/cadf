#ifndef CAMB_NETWORK_SOCKET_SERVER_SOCKET_H
#define CAMB_NETWORK_SOCKET_SERVER_SOCKET_H

#include <sys/socket.h>
#include <arpa/inet.h>

#include "comms/network/socket/AbstractTcpSocket.h"
#include "comms/network/socket/IServerConnectionHandler.h"

#include "thread/Thread.h"

namespace cadf::comms {

    /**
     * A TCP Server Socket that will allow for external connections
     */
    class TcpServerSocket: public AbstractTcpSocket, public cadf::thread::LoopingTask {
        public:
            /**
             * CTOR
             *
             * @param &info const NetworkInfo indicating where the server is to be exposed
             * @param *connectionHandler IServerConnectionHandler where connections to client are to be managed
             * @param connectionQueueSize int how many client connections can be allowed to be concurrently pending waiting to connect
             */
            TcpServerSocket(const NetworkInfo &info, IServerConnectionHandler *connectionHandler, int connectionQueueSize);

            /**
             * DTOR
             */
            virtual ~TcpServerSocket();

            /**
             * A server socket is not intended to send/receive messages, as such this will do nothing.
             *
             * @param *out const OutputBuffer is unused
             * @return bool false
             */
            bool send(const OutputBuffer *out);

            /**
             * Does nothing, the listener is ignored.
             *
             * @param *listener ISocketMessageReceivedListener is unused
             */
            virtual void addMessageListener(ISocketMessageReceivedListener *listener);

            /**
             * Does nothing, the listener is ignored.
             *
             * @param *listener ISocketMessageReceivedListener is unused
             */
            virtual void removeMessageListener(ISocketMessageReceivedListener *listener);

        protected:
            /**
             * Connects the socket as a server
             *
             * @return bool true if the socket was properly started as a server
             */
            virtual bool establishConnection();

            /**
             * Perform cleanup to terminate the handling/detection of connections
             */
            virtual bool terminateConnection();

        private:
            /** The handler for managing connected clients */
            IServerConnectionHandler *m_connectionHandler;
            /** The size of the connection queue the server is to allow for */
            int m_connectionQueueSize;
            /** The thread in which to process inbound connections */
            cadf::thread::Thread m_connectThread;
            /** The size of the address data structure */
            size_t m_addrlen;

            /**
             * The thread loop that waits for a client to connect
             */
            void execLoop();
    };
}

#endif /* CAMB_NETWORK_SOCKET_SERVER_SOCKET_H */
