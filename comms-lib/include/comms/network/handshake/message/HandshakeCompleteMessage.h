#ifndef CAMB_NETWORK_HANDSHAKECOMPLETEMESSAGE_H_
#define CAMB_NETWORK_HANDSHAKECOMPLETEMESSAGE_H_

#include "comms/message/Message.h"

namespace cadf::comms {

    /**
     * The data the HandshakeCompleteMessage contains
     */
    struct HandshakeCompleteData {
            /** The version of the handshake protocol that is supported on the connection */
            unsigned int version;
    };

    /**
     * Verify the equality of two handshake completion data structures.
     *
     * @param &lhs const HandshakeCompleteData on the left side of the comparison
     * @param &rhs const HandshakeCompleteData on the right side of the comparison
     *
     * @return bool true of the two are equal
     */
    bool operator==(const HandshakeCompleteData &lhs, const HandshakeCompleteData &rhs);

    /**
     * Stream the data structure to the provided stream.
     *
     * @param &stream std::ostream to which the data structure is to be streamed
     * @param &data const HandshakeCompleteData that is to be streamed
     */
    std::ostream& operator<<(std::ostream &stream, const HandshakeCompleteData &data);

    /**
     * Message that is used to indicate that a handshake between a client and server connection is now complete.
     */
    class HandshakeCompleteMessage: public AbstractDataMessage<HandshakeCompleteData> {

        public:
            /**
             * CTOR - Default constructor
             */
            HandshakeCompleteMessage() : AbstractDataMessage<HandshakeCompleteData>("HandshakeCompleteMessage", HandshakeCompleteData()) {
            }

            /**
             * CTOR - constructor that specifies the data to use
             *
             * @param &data const HandshakeCompleteData
             */
            HandshakeCompleteMessage(const HandshakeCompleteData &data) : AbstractDataMessage("HandshakeCompleteMessage", data) {
            }

            /**
             * Build a new instance
             *
             * @return AbstractDataMessage<HandshakeCompleteData>* pointing to the newly created copy
             */
            AbstractDataMessage<HandshakeCompleteData>* newInstance() const {
                return new HandshakeCompleteMessage(m_data);
            }

            /**
             * Check if the two messages are equal
             *
             * @return true if they are
             */
            bool equals(const HandshakeCompleteMessage *rhs) {
                return getType() == rhs->getType() && m_data == rhs->m_data;
            }
    };

}

#endif /* CAMB_NETWORK_HANDSHAKECOMPLETEMESSAGE_H_ */
