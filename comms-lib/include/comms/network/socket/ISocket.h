#ifndef CAMB_NETWORK_SOCKET_ISOCKET_H_
#define CAMB_NETWORK_SOCKET_ISOCKET_H_

#include "comms/network/socket/ISocketMessageReceivedListener.h"
#include "comms/network/NetworkInfo.h"

namespace cadf::comms {

    /**
     * Socket that allows for the sending and receiving of data.
     */
    class ISocket {
        public:
            /**
             * DTOR
             */
            virtual ~ISocket() = default;

            /**
             * Connect the socket.
             *
             * @return true if successful
             */
            virtual bool connect() = 0;

            /**
             * Disconnect the socket.
             *
             * @return true if successful
             */
            virtual bool disconnect() = 0;

            /**
             * Check if the socket is currently connected
             *
             * @return true if connected at present
             */
            virtual bool isConnected() = 0;

            /**
             * Send the data contained within the buffer. It is expected that the buffer contains a serialized message.
             *
             * @param *out const OutputBuffer containing the message to send
             * @return bool true if the message was sent successfully
             *
             * A cadf::comms::MessageSendingException will be thrown if an issue is encountered attempting to send the message.
             */
            virtual void send(const OutputBuffer *out) = 0;

            /**
             * Add a listener that is to be triggered when a message is received by the socket.
             *
             * @param *listener ISocketMessageReceivedListener
             */
            virtual void addMessageListener(ISocketMessageReceivedListener *listener) = 0;

            /**
             * Remove a listener to no longer be notified when a message is received.
             *
             * @param *listener ISocketMessageReceivedListener
             */
            virtual void removeMessageListener(ISocketMessageReceivedListener *listener) = 0;
    };
}

#endif /* CAMB_NETWORK_SOCKET_ISOCKET_H_ */
