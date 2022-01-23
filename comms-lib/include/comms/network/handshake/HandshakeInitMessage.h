#ifndef CAMB_NETWORK_HANDSHAKEINITMESSAGE_H_
#define CAMB_NETWORK_HANDSHAKEINITMESSAGE_H_

#include "comms/message/Message.h"

namespace cadf::comms {

    /**
     * The data the init message will contain.
     */
    struct HandshakeInitData {
            /** The maximum version of the handshake protocol that the server can make use of */
            unsigned int maxVersion = 1;
    };

    /**
     * Check if two datas are the same
     *
     * @param &lhs const HandshakeInitData
     * @param &rhs const HandshakeInitData
     * @return bool true if they are the same
     */
    bool operator==(const HandshakeInitData &lhs, const HandshakeInitData &rhs);

    /**
     * Stream the data into the stream.
     *
     * @param &stream std::ostream to stream the message to
     * @param &data const HandshakeInitData to stream
     * @return std::ostream& to which the data was streamed
     */
    std::ostream& operator<<(std::ostream &stream, const HandshakeInitData &data);

    /**
     * The message with which the handshake process is started
     */
    class HandshakeInitMessage: public AbstractDataMessage<HandshakeInitData> {

        public:
            /**
             * CTOR - the handshake always indicates the highest version of the protocol that the server supports
             */
            HandshakeInitMessage() : AbstractDataMessage<HandshakeInitData>("HandshakeInitMessage", HandshakeInitData()) {
            }

            /**
             * Create a new instance of the message
             *
             * @return AbstractDataMessage<HandshakeInitData>
             */
            AbstractDataMessage<HandshakeInitData>* newInstance() const {
                return new HandshakeInitMessage();
            }

            /**
             * Check if the message has the same contents
             *
             * @param *rhs const HandshakeInitMessage to compare to
             * @return bool true if they are the same
             */
            bool equals(const HandshakeInitMessage *rhs) {
                return getType() == rhs->getType() && m_data == rhs->m_data;
            }
    };

}

#endif /* CAMB_NETWORK_HANDSHAKEINITMESSAGE_H_ */
