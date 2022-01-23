#include "comms/network/handshake/HandshakeHandler.h"

namespace cadf::comms {
    /*
     * CTOR
     */
    HandshakeTerminator::HandshakeTerminator(HandshakeHandler *handler, IHandshake *handshake, IHandshakeCompleteListener *listener) : m_handler(handler), m_handshake(handshake), m_listener(listener) {
    }

    /*
     * Terminates the handshake and triggers the memory cleanup
     */
    void HandshakeTerminator::handshakeComplete(int type, int instance, ISocketDataHandler *socket) {
        m_listener->handshakeComplete(type, instance, socket);
        m_handler->cleanup(this);
    }

    /*
     * CTOR
     */
    HandshakeHandler::HandshakeHandler(IHandshakeFactory *handshakeFactory) : m_handshakeFactory(handshakeFactory) {

    }

    /*
     * Starts the handshake
     */
    void HandshakeHandler::performHandshake(int socketFd, IHandshakeCompleteListener *completionListener) {
        IHandshake *handshake = m_handshakeFactory->create(socketFd);
        HandshakeTerminator *terminator = new HandshakeTerminator(this, handshake, completionListener);
        m_terminators[terminator] = handshake;
        handshake->start(terminator);
    }

    /*
     * Cleanup after the completion of a handshake
     */
    void HandshakeHandler::cleanup(HandshakeTerminator *done) {
        delete (m_terminators[done]);
        m_terminators.erase(done);
        delete (done);
    }
}
