#ifndef CAMB_NETWORK_SOCKET_CLIENTSOCKET_H_
#define CAMB_NETWORK_SOCKET_CLIENTSOCKET_H_

#include "comms/network/socket/AbstractTcpSocket.h"
#include "comms/network/socket/TcpSocketDataHandler.h"

#include <set>

namespace cadf::comms {

    /**
     * A TCP Client that will connect to a TCP Server and then allows for message passing between the two.
     */
    class TcpClientSocket: public AbstractTcpSocket {
        public:
            /**
             * CTOR
             *
             * @param &info const NetworkInfo with details of where to connect to the server
             * @param maxMessageSize size_t
             */
            TcpClientSocket(const NetworkInfo &info, size_t maxMessageSize);

            /**
             * DTOR
             */
            virtual ~TcpClientSocket();

            /**
             * Send the data in the message
             *
             * @param *out const OutputBuffer with the message to send
             *
             * A cadf::comms::SocketException will be thrown if an issue is encountered attempting to send the message.
             */
            virtual void send(const OutputBuffer *out);

            /**
             * Add a listener that is to be triggered when a message is received by the socket.
             *
             * @param *listener ISocketMessageReceivedListener
             */
            virtual void addMessageListener(ISocketMessageReceivedListener *listener);

            /**
             * Remove a listener to no longer be notified when a message is received.
             *
             * @param *listener ISocketMessageReceivedListener
             */
            virtual void removeMessageListener(ISocketMessageReceivedListener *listener);

        protected:
            /**
             * Establish the connection to the server.
             *
             * @return bool true if the connection was successfully established
             */
            virtual bool establishConnection();

            /**
             * Terminates the connection to the server
             *
             * @return bool true if the connection was successfully terminated
             */
            virtual bool terminateConnection();

        private:
            /** The maximum size of a message that can be sent/receive */
            size_t m_maxMessageSize;
            /** All listeners that have been registered */
            std::set<ISocketMessageReceivedListener*> m_listeners;
            /** For processing the data on the socket */
            ISocketDataHandler *m_dataSocket;

    };

}

#endif /* CAMB_NETWORK_SOCKET_CLIENTSOCKET_H_ */
