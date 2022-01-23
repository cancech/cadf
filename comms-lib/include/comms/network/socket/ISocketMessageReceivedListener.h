#ifndef CAMB_NETWORK_INETWORKMESSAGERECEIVEDLISTENER_H_
#define CAMB_NETWORK_INETWORKMESSAGERECEIVEDLISTENER_H_

#include "comms/network/Buffer.h"

namespace cadf::comms {

    /**
     * Listener to be notified when a message is recieved on a socket.
     */
    class ISocketMessageReceivedListener {

        public:
            /**
             * DTOR
             */
            virtual ~ISocketMessageReceivedListener() = default;

            /**
             * Called when a message is received
             *
             * @param *in InputBuffer containing the received message
             */
            virtual void messageReceived(InputBuffer *in) = 0;
    };

}

#endif /* CAMB_NETWORK_INETWORKMESSAGERECEIVEDLISTENER_H_ */
