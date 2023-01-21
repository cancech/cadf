#ifndef CAMB_NETWORK_HANDSHAKERESPONSEMESSAGE_H_
#define CAMB_NETWORK_HANDSHAKERESPONSEMESSAGE_H_

#include "comms/message/Message.h"

namespace cadf::comms {

    /**
     * The data that is to be provided with the handshake response.
     *
     * This contains the data as required for version 1 of the handshake protocol.
     */
    struct HandshakeResponseDataV1 {
            /** The number of the client connection type */
            int clientType;
            /** The number of the client connection instance */
            int clientInstance;
    };

    /**
     * Check if two datas are the same
     *
     * @param &lhs const HandshakeResponseDataV1
     * @param &rhs const HandshakeResponseDataV1
     * @return bool true if they are the same
     */
    bool operator==(const HandshakeResponseDataV1 &lhs, const HandshakeResponseDataV1 &rhs);

    /**
     * Stream the data into the stream.
     *
     * @param &stream std::ostream to stream the message to
     * @param &data const HandshakeResponseDataV1 to stream
     * @return std::ostream& to which the data was streamed
     */
    std::ostream& operator<<(std::ostream &stream, const HandshakeResponseDataV1 &data);

    /**
     * The message with which the client response to the handshake request.
     *
     * This is the message for version 1 of the handshake protocol.
     */
    class HandshakeResponseMessageV1: public AbstractDataMessage<HandshakeResponseDataV1> {

        public:
            /**
             * CTOR - default
             */
            HandshakeResponseMessageV1() : AbstractDataMessage<HandshakeResponseDataV1>("HandshakeResponseMessageV1", HandshakeResponseDataV1()) {
            }

            /**
             * CTOR
             *
             * @param &data const HandshakeResponseDataV1 the data the message should contain
             */
            HandshakeResponseMessageV1(const HandshakeResponseDataV1 &data) : AbstractDataMessage<HandshakeResponseDataV1>("HandshakeResponseMessageV1", data) {
            }

            /**
             * Create a new instance of the message
             *
             * @return AbstractDataMessage<HandshakeResponseDataV1>
             */
            AbstractDataMessage<HandshakeResponseDataV1>* newInstance() const {
                return new HandshakeResponseMessageV1(m_data);
            }

            /**
             * Check if the message has the same contents
             *
             * @param *rhs const HandshakeResponseMessageV1 to compare to
             * @return bool true if they are the same
             */
            bool equals(const HandshakeResponseMessageV1 *rhs) {
                return getType() == rhs->getType() && m_data == rhs->m_data;
            }
    };

}

#endif /* CAMB_NETWORK_HANDSHAKERESPONSEMESSAGE_H_ */
