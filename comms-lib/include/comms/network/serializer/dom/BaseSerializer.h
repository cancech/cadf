#ifndef CAMB_NETWORK_JSON_SERIALIZER_H_
#define CAMB_NETWORK_JSON_SERIALIZER_H_

#include "comms/network/serializer/dom/SerializationFuncs.h"
#include "comms/network/serializer/TemplateProtocol.h"
#include "comms/network/NetworkException.h"
#include "comms/message/Message.h"
#include "comms/message/MessageFactory.h"
#include "dom/DomConverter.h"

namespace cadf::comms::dom {

    /**
     * Serializer that is responsible for converting an AbstractDataMessage class to a JSON representation
     *
     * @template T the class (struct) representing the data that is stored within the message
     */
    template<class T>
    class Serializer: public ISerializer {

        public:
            /**
             * CTOR
             *
             * @param *msg const AbstractDataMessage that is to be serialized
             */
            Serializer(cadf::dom::DomConverter *converter, const AbstractDataMessage<T> *msg, int type, int instance)
                    : ISerializer(msg->getType()), m_type(type), m_instance(instance) {
                cadf::dom::DomNode root = cadf::dom::buildNode("message", m_msgType);
                root["type"] = m_type;
                root["instance"] = m_instance;
                root["data"] = buildTree<T>(msg->getData());
                m_stringRep = converter->toString(root);
            }

            /**
             * Determine the size of the message when it is serialized to JSON.
             *
             * @return size_t the number of bytes (characters) required in order to properly serialize the message
             */
            size_t getSize() const {
                return m_stringRep.size() + 1; // +1 for null terminator
            }

            /**
             * Serialize the data.
             *
             * @param *buffer OutputBuffer pointer where the data is to be copied to
             *
             * Note: this is dependent on pre-existing external buildTree functions being available for the population of the DOM tree with the data.
             */
            void serialize(OutputBuffer *buffer) {
                buffer->append(m_stringRep.c_str(), getSize());
            }

        private:
            /** The representation of the message as a string */
            std::string m_stringRep;
            // The type of recipient
            int m_type;
            // The instance of the type
            int m_instance;
    };

    /**
     * Deserializer that is responsible for re-interpreting JSON to reconstruct the data from it
     */
    class Deserializer: public IDeserializer {

        public:
            /**
             * CTOR
             *
             * @param *buffer InputBuffer pointer to the buffer where the received JSON is stored
             */
            Deserializer(cadf::dom::DomConverter *converter, InputBuffer *buffer);

            /**
             * DTOR
             */
            virtual ~Deserializer() = default;

            /**
             * Load the data from the message and populate a data structure with it.
             *
             * @template T the type of class (struct) where the data is contained
             * @return T the data structure with the loaded data
             *
             * Note: this is dependent on pre-existing external loadFromBuilder functions being available for the population of the data type.
             */
            template<class T>
            T getData() const {
                return loadFromTree<T>(m_root["data"]);
            }

        private:
            // The root of the loaded JSON tree created from the JSON string
            cadf::dom::DomNode m_root;
    };
}

#endif /* CAMB_NETWORK_JSON_SERIALIZER_H_ */
