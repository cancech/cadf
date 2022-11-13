#ifndef CAMB_NETWORK_CLIENT_H_
#define CAMB_NETWORK_CLIENT_H_

#include "comms/network/Buffer.h"
#include "comms/network/socket/ISocketMessageReceivedListener.h"

namespace cadf::comms {

    /**
     * Interface for a client that can connect to a server and pass messages back and forth. Messages to be sent must first be serialized into an OutputBuffer and received messages will be passed to
     * the specified ISocketMessageReceivedListener which will have to perform any required deserialization on the received message.
     */
    class IClient {
        public:
            /**
             * DTOR
             */
            virtual ~IClient() = default;

            /**
             * Specify the listener which is to be notified when a message has been received.
             *
             * @param *listener ISocketMessageReceivedListener
             */
            virtual void setListener(ISocketMessageReceivedListener *listener) = 0;

            /**
             * Attempt to connect the client to the server.
             *
             * @return bool true if the connection was made successfully.
             */
            virtual bool connect() = 0;

            /**
             * Attempt to disconnect the client to the server.
             *
             * @return bool true if the connection was successfully terminated.
             */
            virtual bool disconnect() = 0;

            /**
             * Check whether the client is currently connected.
             *
             * @return bool true if connected
             */
            virtual bool isConnected() = 0;

            /**
             * Send a message to the server.
             *
             * @param *out OutputBuffer containing the serialized message to send
             *
             * A cadf::comms::MessageSendingException will be thrown if an issue is encountered attempting to send the message.
             */
            virtual void send(OutputBuffer *out) = 0;
    };
}

#endif /* CAMB_NETWORK_CLIENT_H_ */
