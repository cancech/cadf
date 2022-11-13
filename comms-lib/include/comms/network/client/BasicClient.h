#ifndef CAMB_NETWORK_SIMPLECLIENT_H_
#define CAMB_NETWORK_SIMPLECLIENT_H_


#include "comms/network/socket/TcpClientSocket.h"
#include "comms/network/client/Client.h"

namespace cadf::comms {

    /**
     * A default client implementation that will connect to a server
     */
    class BasicClient: public IClient {
        public:
            /**
             * CTOR
             *
             * @param *socket TcpClientSocket wrapper for the socket on which to connect to the server
             */
            BasicClient(TcpClientSocket *socket);

            /**
             * DTOR
             */
            virtual ~BasicClient();

            /**
             * Specify the listener which is to be notified when a message has been received.
             *
             * @param *listener ISocketMessageReceivedListener
             */
            virtual void setListener(ISocketMessageReceivedListener *listener);

            /**
             * Attempt to connect the client to the server.
             *
             * @return bool true if the connection was made successfully.
             */
            virtual bool connect();

            /**
             * Attempt to disconnect the client to the server.
             *
             * @return bool true if the connection was successfully terminated.
             */
            virtual bool disconnect();

            /**
             * Check whether the client is currently connected.
             *
             * @return bool true if connected
             */
            virtual bool isConnected();

            /**
             * Send a message to the server.
             *
             * @param *out OutputBuffer containing the serialized message to send
             *
             * A cadf::comms::SocketException will be thrown if an issue is encountered attempting to send the message.
             */
            virtual void send(OutputBuffer *out);

        private:
            /** The connection socket */
            TcpClientSocket *m_socket;
            /** The listener to notify of received messages */
            ISocketMessageReceivedListener *m_messageProcessor;
    };
}

#endif /* CAMB_NETWORK_SIMPLECLIENT_H_ */
