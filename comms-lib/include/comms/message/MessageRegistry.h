#ifndef CAMB_MESSAGE_MESSAGEREGISTRY_H_
#define CAMB_MESSAGE_MESSAGEREGISTRY_H_

#include "comms/message/MessageFactory.h"

namespace cadf::comms {

    /**
     * Registry which stores a list of messages that are to be registered. Each value passed into the template
     * represents a single message to register.
     *
     * @template PROTOCOL the class which defines how messages will be (de)serialized for transmission over the network
     * @template Msgs... arbitrary list of messages that are to be registered. Each must extend from the base IMessage class.
     */
    template<class PROTOCOL, class ... Msgs>
    class MessageRegistry {

        public:
            /**
             * DTOR
             */
            virtual ~MessageRegistry() = default;

            /**
             * Registers the stored message types with the provided MessageFactory.
             *
             * @param MessageFactory* pointer to the factory that is to have the stored messages registered.
             */
            virtual void registerMessages(MessageFactory<PROTOCOL> *factory) {
                registerAllMessages<Msgs...>(factory);
            }

        private:
            /**
             * Recursive method that iterates through all of the stored message types and registers each
             * with the factory, including the serializaton factory as provided by the protocol.
             *
             * @template Msg the current message to be registered
             * @template RemainingMsgs the list of messages that have not yet been registered
             *
             * @param MessageFactory* pointer to the factory that is to have the stored messages registered.
             */
            template<class Msg, class ... RemainingMsgs>
            void registerAllMessages(MessageFactory<PROTOCOL> *factory) {
                registerMessage<Msg>(factory);
                registerAllMessages<RemainingMsgs...>(factory);
            }

            /**
             * The end of recursion method, which is called when there are no more messages to register.
             *
             * @template Null indicating that there are no more messages to register
             *
             * @param MessageFactory* pointer to the factory that is to have the stored messages registered.
             */
            template<int = 0>
            void registerAllMessages(MessageFactory<PROTOCOL> *factory) {
            }

            /**
             * Register the MESSAGE and its serializer with the factory.
             *
             * @param *factory MessageFactory for the protocol where the message and its serializer are to be registered
             * @template MESSAGE the class of the message that is to registered
             */
            template<class MESSAGE>
            void registerMessage(MessageFactory<PROTOCOL> *factory) {
                MESSAGE *msg = new MESSAGE();
                ISerializerFactory* serializerFactory = PROTOCOL::createSerializerFactory(msg);

                try {
                    factory->registerMessage(msg, serializerFactory);
                } catch (std::exception &ex) {
                    delete(msg);
                    delete(serializerFactory);
                    throw;
                }
            }
    };

}

#endif /* CAMB_MESSAGE_MESSAGEREGISTRY_H_ */
