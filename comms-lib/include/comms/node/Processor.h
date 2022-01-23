#ifndef CAMB_PROCESSOR_H_
#define CAMB_PROCESSOR_H_

#include "comms/message/Message.h"
#include "comms/message/MessageException.h"

namespace cadf::comms {

    /**
     * Interface that all receivers must implement.
     */
    class IProcessor {

        public:
            /**
             * DTOR
             */
            virtual ~IProcessor() = default;

            /**
             * Get the type of message that the receiver wants to receive.
             *
             * @return std::string indicating the message type.
             */
            virtual std::string getType() = 0;

            /**
             * Receive and process the message.
             *
             * The sender is responsible for any memory management, and it can be safely assumed that the message
             * will cease to exist as soon as this method returns. Therefore it is recommended to either consume
             * the message in its entirety as part of the process() call, or to copy out any data from within that
             * is to be kept.
             *
             * @param msg const Message* pointer to the message that is to be received.
             */
            virtual void process(const IMessage *msg) = 0;
    };

    /**
     * IReceiver implementation for processing Messages of a particular type, as specified in the template.
     * Upon receipt of message, it will convert the abstract message to the particular type, and pass it on
     * to the pure virtual processMessage for processing. If the conversion fails, an exception will be thrown.
     */
    template<class T>
    class MessageProcessor: public IProcessor {
        public:
            /**
             * DTOR
             */
            virtual ~MessageProcessor() = default;

            /**
             * Get the type of message that this receiver is to process, from the message itself.
             *
             * @return std::string get the type of message this is to process.
             */
            virtual std::string getType() {
                return T().getType();
            }

            /**
             * Receive the message and attempt to convert it to the message type the receiver wants to process. If
             * successful, the message is passed to the subclass for processing, otherwise an exception is thrown.
             *
             * @param *msg const IMessage received and to be processed
             */
            virtual void process(const IMessage *msg) {
                if (const T *castMsg = dynamic_cast<const T*>(msg))
                    processMessage(castMsg);
                else
                    throw InvalidMessageTypeException(msg->getType(), "Failed to convert the received message to this type");
            }

        protected:
            /**
             * Pure virtual method, where a subclass must provide the implementation for how to processes the message
             * that was received.
             *
             * @param msg T* const message converted to the expected Message class that is to be processed.
             */
            virtual void processMessage(const T *msg) = 0;
    };

}

#endif /* CAMB_PROCESSOR_H_ */
