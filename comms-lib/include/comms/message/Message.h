#ifndef CAMB_MESSAGE_MESSAGE_H_
#define CAMB_MESSAGE_MESSAGE_H_

#include <iostream>
#include <string>


namespace cadf::comms {

    /**
     * The base message class from which all messages must derive
     */
    class IMessage {

        public:
            /**
             * DTOR
             */
            virtual ~IMessage() = default;

            /**
             * Get the type of the message as a string
             *
             * @return std::string the type of the message
             */
            virtual std::string getType() const = 0;

            /**
             * Create a clone of the message.
             *
             * @return Message* pointer to the newly created clone of this message.
             */
            virtual IMessage* clone() const = 0;
    };

    /**
     * A Message that contains data. The data is to be contained within a struct
     * of type T as denoted in the template. This message contains virtually all of the
     * implementation required, however it is expected to be subclassed to ensure
     * proper operation.
     */
    template<class T>
    class AbstractDataMessage: public IMessage {

        public:
            /**
             * CTOR
             *
             * @param type std::string the type of the message
             * @param &data const reference to the data the message is to contain
             */
            AbstractDataMessage(std::string type, const T &data) : m_type(type), m_data(data) {
            }

            /**
             * DTOR
             */
            virtual ~AbstractDataMessage() = default;

            /**
             * Get the type of this message
             *
             * @return std::string the type
             */
            virtual std::string getType() const {
                return m_type;
            }

            /**
             * Create a cloned copy of this message, that is identical to this one.
             *
             * @return IMessage* the newly allocated cloned copy
             */
            virtual IMessage* clone() const {
                return newInstance();

            }

            /**
             * Apply a new copy of the data to the message.
             *
             * *param &data const T reference to the data
             */
            void setData(const T &data) {
                m_data = data;
            }

            /**
             * Get the data within the message.
             *
             * @return const reference to the data (T)
             */
            const T& getData() const {
                return m_data;
            }

        private:
            /** The std::string denoting the type of message */
            std::string m_type;

        protected:
            /** The data of the message */
            T m_data;

            /**
             * Helper to create a new message class instance for cloning. To be overridden by subclass to ensure the
             * correct subclass type is created.
             *
             * @return DataMessage<T>* pointer to the newly created message instance
             */
            virtual AbstractDataMessage<T>* newInstance() const = 0;
    };

    /**
     * Concrete implementation of the AbstractDataMessage for situation where a full fledged subclass is not required.
     * This provides the missing implementation, but it should not be subclasses. For subclassing use the base
     * AbstractDataMessage.
     */
    template<class T>
    class DataMessage: public AbstractDataMessage<T> {
        public:
            /**
             * CTOR
             *
             * @param type std::string the type of the message
             */
            DataMessage(std::string type, const T &data) : AbstractDataMessage<T>(type, data) {
            }

            /**
             * DTOR
             */
            virtual ~DataMessage() = default;

        protected:
            /**
             * Creates a new instance.
             *
             * @return pointer to the new instance as an AbstractDataMessage
             */
            virtual AbstractDataMessage<T>* newInstance() const {
                return new DataMessage(AbstractDataMessage<T>::getType(), AbstractDataMessage<T>::m_data);
            }
    };
}

#endif /* CAMB_MESSAGE_MESSAGE_H_ */
