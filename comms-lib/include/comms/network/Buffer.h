#ifndef CAMB_NETWORK_BUFFER_H_
#define CAMB_NETWORK_BUFFER_H_

#include <vector>
#include <string.h>
#include <iostream>

#include "comms/network/NetworkException.h"

namespace cadf::comms {

    /**
     * Buffer for storing data that is in the process of being transmitted.
     */
    class Buffer {
        public:
            /**
             * DTOR
             */
            virtual ~Buffer();

            /**
             * Get the size of the Data stored within the buffer.
             *
             * @return size_t indicating the size of the data within the buffer.
             */
            size_t getDataSize() const {
                return m_dataSize;
            }

            /**
             * Get the Total size of the buffer (maximum amount of data that it can contain.
             *
             * @return size_t indicating the size of the total size of the buffer.
             */
            size_t getTotalSize() const {
                return m_totalSize;
            }

            /**
             * Get the pointer to the start of the data within the buffer.
             *
             * @return char* pointing to the start of the buffer data
             */
            const char* getData() const {
                return m_buffer;
            }

        protected:
            // The size of the data (in bytes)
            size_t m_dataSize;
            // The size of the buffer (in bytes)
            size_t m_totalSize;
            // The start of the buffer
            char *m_buffer;
            // The index/offset where the next buffer operation will take place
            size_t m_currIndex;

            /**
             * CTOR
             *
             * Creates an empty buffer of the indicated size.
             *
             * @param size_t how large of a buffer to create.
             */
            Buffer(size_t size);

            /**
             * CTOR
             *
             * Create a new buffer to match the specified data.
             *
             * @param *data const char pointer to the existing buffer
             * @param size size_t how large the buffer is
             */
            Buffer(const char *data, size_t size);
    };

    /**
     * Buffer for performing output (i.e.: writing data into the buffer).
     */
    class OutputBuffer: public Buffer {

        public:
            /**
             * CTOR
             *
             * Creates a buffer that will accommodate up to size amount of bytes.
             *
             * @param size size_t the size of the buffer.
             */
            OutputBuffer(size_t size);

            /**
             * DTOR
             */
            virtual ~OutputBuffer() = default;

            /**
             * Appends data to the buffer. This will be placed at the end of the already appended data.
             *
             * @template T typename of the data that is to be appended
             *
             * @param &data const reference to the data (type T) that is to be appended
             * @param dataSize size_t indicating how much data is to be appended
             */
            template<typename T>
            void append(const T &data, size_t dataSize) {
                AppendValue<T>::append(this, data, dataSize);
                m_dataSize += dataSize;
            }

        private:
            /**
             * Helper struct that "translates" scalar data in preparation of appending it to the buffer
             */
            template<typename T>
            struct AppendValue {
                    static void append(OutputBuffer *buffer, const T &data, const size_t &dataSize) {
                        buffer->appendToBuffer(&data, dataSize);
                    }
            };

            /**
             * Helper struct that "translates" pointer data in preparation of appending it to the buffer
             */
            template<typename T>
            struct AppendValue<T*> {
                    static void append(OutputBuffer *buffer, const T *data, const size_t &dataSize) {
                        buffer->appendToBuffer(data, dataSize);
                    }
            };

            /**
             * Appends the data to buffer after the last added piece of data.
             *
             * @param *data const pointer to the data (type T) that is to be appended
             * @param dataSize size_t indicating how much data is being appended
             */
            template<typename T>
            void appendToBuffer(const T *data, const size_t &dataSize) {
                size_t nextSize = m_currIndex + dataSize;
                if (nextSize > m_totalSize)
                    throw BufferOverflowException();

                memcpy(m_buffer + m_currIndex, data, dataSize);
                m_currIndex  = nextSize;
            }
    };

    /**
     * Buffer for performing input (i.e.: loading data from buffer).
     */
    class InputBuffer: public Buffer {

        public:
            /**
             * CTOR
             *
             * Creates a new buffer based on the provided one.
             *
             * @param *data const char pointer to the existing buffer
             * @param size size_t how large the buffer is
             */
            InputBuffer(const char *buffer, size_t size);

            /**
             * DTOR
             */
            ~InputBuffer() = default;

            /**
             * Retrieve the next piece of data from the buffer.
             *
             * @template T typename indicating what kind of data is being retrieved
             *
             * @param dataSize size_t the size of the data to retrieve
             *
             * @return the data as retrieved from the buffer
             */
            template<typename T>
            T retrieveNext(std::size_t dataSize) {
                return RetrieveValue<T>::value(this, dataSize);
            }

        private:
            /**
             * Helper struct for retrieving scalar data from the buffer
             */
            template<typename T>
            struct RetrieveValue {
                    static T value(InputBuffer *buffer, const size_t &dataSize) {
                        T retVal;
                        buffer->nextValue(&retVal, dataSize);
                        return retVal;
                    }
            };

            /**
             * Helper struct for retrieving pointer data from the buffer
             */
            template<typename T>
            struct RetrieveValue<T*> {
                    static T* value(InputBuffer *buffer, const size_t &dataSize) {
                        T *retVal = new T();

                        try {
                            buffer->nextValue(retVal, dataSize);
                            return retVal;
                        } catch (std::exception &ex) {
                            delete(retVal);
                            throw;
                        }
                    }
            };

            /**
             * Retrieve the next value from the buffer
             *
             * @param *destination pointer (type T) to where the data is to be stored
             * @param &dataSize const size_t indicating how much data to pull from the buffer
             */
            template<typename T>
            void nextValue(T *destination, const size_t &dataSize) {
                size_t nextSize = m_currIndex + dataSize;
                if (nextSize > m_totalSize)
                    throw BufferOverflowException();

                memcpy(destination, m_buffer + m_currIndex, dataSize);
                m_currIndex = nextSize;
            }
    };
}

#endif /* CAMB_NETWORK_BUFFER_H_ */
