#ifndef CAMB_MESSAGE_MESSAGEFACTORY_H_
#define CAMB_MESSAGE_MESSAGEFACTORY_H_

#include <type_traits>
#include <string>
#include <map>
#include <iostream>
#include <memory>

#include "comms/Constants.h"
#include "comms/network/serializer/Serializer.h"
#include "comms/message/MessagePacket.h"
#include "comms/message/MessageException.h"

namespace cadf::comms {

    /**
     * Interface for the factory class which allows for creating and storing all supported messages
     */
    struct IMessageFactory {
            /**
             * DTOR
             */
            virtual ~IMessageFactory() = default;

            /**
             * Check if a message of the type has been registered with the factory.
             *
             * @param type std::string the type of message to look for
             *
             * @return bool true if it has been registered
             */
            virtual bool isMessageRegistered(std::string type) const = 0;
    };

    /**
     * Factory for storing and creating all known and supported messages, as well as providing access to the means for serializing and deserializing them.
     *
     * @template PROTOCOL the class which defines how messages will be (de)serialized for transmission over the network
     */
    template<class PROTOCOL>
    class MessageFactory: public IMessageFactory {

        public:
            /**
             * CTOR
             *
             * @param bufferSize size_t unsigned integer size to use for the buffer when serializing messages. Defaults to MessageConstants::AUTO_SIZE, which
             *                          will automatically determine the size of the buffer based on the size of the serialized data
             */
            MessageFactory(size_t bufferSize = MessageConstants::AUTO_SIZE): m_bufferSize(bufferSize) {
            }

            /**
             * DTOR
             */
            virtual ~MessageFactory() {
                for (auto iter = m_messages.begin(); iter != m_messages.end(); ++iter)
                    delete (iter->second);
                for (auto iter = m_serializers.begin(); iter != m_serializers.end(); ++iter)
                    delete (iter->second);
            }

            /**
             * Register a new message with the factory, along with the SerializerFactory for the message
             *
             * Note that the message must be allocated to both, and on destruction the MessageFactory will clean up the memory.
             *
             * @param *message IMessage pointer to the message that is to be registered
             * @param *factory ISerializerFactory pointer to the factory for (de)serializing messages of this type as per the specified protocol
             * @throws InvalidMessageTypeException if a message of this type is already registered
             */
            virtual void registerMessage(IMessage *message, ISerializerFactory *factory) {
                std::string type = message->getType();
                if (m_messages.count(type))
                    throw InvalidMessageTypeException(type, "Already registered with factory");

                m_messages[type] = message;
                m_serializers[type] = factory;
            }

            /**
             * Check if a message of the type has been registered with the factory.
             *
             * @param type std::string the type of message to look for
             *
             * @return bool true if it has been registered
             */
            virtual bool isMessageRegistered(std::string type) const {
                return m_messages.count(type);
            }

            /**
             * Creates a new message of the specified type.
             *
             * @param type std::string the type of message to create
             * @return T* newly allocated and created message of the specified type
             * @throws InvalidMessageTypeException if no message of the type is registered, or if the
             *         desired Message class does not match the registered message class.
             */
            virtual IMessage* createMessage(std::string type) const {
                // Try to create the message
                try {
                    return m_messages.at(type)->clone();
                } catch (std::out_of_range &ex) {
                    throw InvalidMessageTypeException(type, "Not registered with factory");
                }
            }

            /**
             * Serialize the specified message to a buffer so that it can be sent out.
             *
             * @param *msg const IMessage to be serialized
             * @return OutputBuffer* containing the serialized message. Note: allocated memory must be managed by the caller.
             */
            virtual OutputBuffer* serializeMessage(const MessagePacket &packet) const {
                const IMessage *msg = packet.getMessage();
                std::unique_ptr<ISerializer> serializer(m_serializers.at(msg->getType())->buildSerializer(msg, packet.getRecipientType(), packet.getRecipientInstance()));
                OutputBuffer *out = new OutputBuffer(m_bufferSize == MessageConstants::AUTO_SIZE ? serializer->getSize() : m_bufferSize);
                try {
                    serializer->serialize(out);
                    return out;
                } catch (std::exception &ex) {
                    delete (out);
                    throw;
                }
            }

            /**
             * Deserialize the data in the buffer and create the corresponding message from it.
             *
             * @param *in InputBuffer with the serialized data for the message
             * @return IMessage* as deserialized from the buffer. Note: allocated memory must be managed by the caller.
             */
            virtual MessagePacket* deserializeMessage(InputBuffer *in) const {
                std::unique_ptr<IDeserializer> deserializer(PROTOCOL::createDeserializer(in));
                IMessage *msg = createMessage(deserializer->getMessageType());
                MessagePacket *packet = new MessagePacket(msg, deserializer->getRecipientType(), deserializer->getRecipientInstance(), true);

                try {
                    m_serializers.at(deserializer->getMessageType())->deserializeTo(msg, deserializer.get());
                    return packet;
                } catch (std::exception &ex) {
                    delete (packet);
                    throw;
                }
            }

        private:
            // Mapping of all registered messages to their type
            std::map<std::string, IMessage*> m_messages;
            // Mapping of all serializer factories to message types
            std::map<std::string, const ISerializerFactory*> m_serializers;
            // The size of the buffer to allocate
            size_t m_bufferSize;
    };

}

#endif /* CAMB_MESSAGE_MESSAGEFACTORY_H_ */
