#ifndef CAMB_NETWORK_HANDSHAKE_HANDSHAKEHANDLER_H_
#define CAMB_NETWORK_HANDSHAKE_HANDSHAKEHANDLER_H_

#include <map>

#include "comms/network/handshake/Handshake.h"

namespace cadf::comms {

    class HandshakeHandler;

    /**
     * Terminator which awaits the completion of the handshake and notifies that it has been completed. This intermediate class is required to help with
     * memory management.
     */
    class HandshakeTerminator: public IHandshakeCompleteListener {
        public:

            /**
             * CTOR
             *
             * @param *handler HandshakeHandler that is responsible for performing and management of all handshakes
             * @param *handshake IHandshake that will perform the handshake itself
             * @param *listener IHandshakeCompleteListener to notify when the handshake is completed
             */
            HandshakeTerminator(HandshakeHandler *handler, IHandshake *handshake, IHandshakeCompleteListener *listener);

            /**
             * DTOR
             */
            virtual ~HandshakeTerminator() = default;

            /**
             * Called when the handshake is completed
             *
             * @param type int of the connection as determined by the handshake
             * @param instance int of the connection as determined by the handshake
             * @param *socket ISocketDataHandler through which to communicate with the external connection
             */
            virtual void handshakeComplete(int type, int instance, ISocketDataHandler *socket);

        private:
            /** The handler for handshakes */
            HandshakeHandler *m_handler;
            /** The handshake itself */
            IHandshake *m_handshake;
            /** Listens for the completion of the handshake */
            IHandshakeCompleteListener *m_listener;
    };

    /**
     * Handles the handshake, making sure that it is started and the listener waiting to hear about the completion of the handshake is notified.
     */
    class HandshakeHandler {
            friend class HandshakeTerminator;

        public:
            /**
             * CTOR
             *
             * @param *handshakeFactory IHandshakeFactory through which the handshakes are created
             */
            HandshakeHandler(IHandshakeFactory *handshakeFactory);

            /**
             * DTOR
             */
            virtual ~HandshakeHandler() = default;

            /**
             * Start a handshake for the given socket, with the listener being notified when the handshake completes.
             *
             * @param socketFd int the file descriptor of the socket where a new connection has been made
             * @param *completionListener IHandshakeCompleteListener to notify when the handshake completes
             */
            virtual void performHandshake(int socketFd, IHandshakeCompleteListener *completionListener);

        protected:
            /**
             * To be called upon the completion of the a handshake, will cleanup memory and the like for the completed handshake.
             *
             * @param *done HandshakeTerminator that completed
             */
            virtual void cleanup(HandshakeTerminator *done);

        private:
            /** Factory for creating the mechanism for performing handshakes for a new connection */
            IHandshakeFactory *m_handshakeFactory;
            /** Map of pending handshake terminators to the handshake they are terminating */
            std::map<HandshakeTerminator*, IHandshake*> m_terminators;
    };
}

#endif /* CAMB_NETWORK_HANDSHAKE_HANDSHAKEHANDLER_H_ */
