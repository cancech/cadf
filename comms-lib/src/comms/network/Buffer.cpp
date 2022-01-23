#include <comms/network/Buffer.h>

namespace cadf::comms {
    /**
     * CTOR - create a buffer of the given size.
     */
    Buffer::Buffer(size_t size) : m_bufferSize(size), m_buffer(new char[size]), m_currIndex(0) {
    }

    /**
     * CTOR - create a buffer from the provided data.
     */
    Buffer::Buffer(const char *buffer, size_t size) : m_bufferSize(size), m_buffer(new char[size]), m_currIndex(0) {
        memcpy(m_buffer, buffer, m_bufferSize);
    }

    /**
     * DTOR
     */
    Buffer::~Buffer() {
        delete[] (m_buffer);
    }

    /**
     * CTOR - for output buffer (output always starts empty)
     */
    OutputBuffer::OutputBuffer(size_t size) : Buffer(size) {
    }

    /**
     * CTOR - for input buffer (input always starts from existing data)
     */
    InputBuffer::InputBuffer(const char *buffer, size_t size) : Buffer(buffer, size) {
    }
}
