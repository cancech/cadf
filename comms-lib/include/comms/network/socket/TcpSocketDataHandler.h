#ifndef CAMB_NETWORK_SOCKET_DATASOCKET_H_
#define CAMB_NETWORK_SOCKET_DATASOCKET_H_

#include <unistd.h>
#include <vector>

#include "comms/network/socket/ISocketMessageReceivedListener.h"
#include "thread/Thread.h"

namespace cadf::comms {

    class ISocketDataHandler {
        public:
            /**
             * DTOR
             */
            virtual ~ISocketDataHandler() = default;

            /**
             * Add a listener to be notified when a message is received.
             *
             * @param *listener ISocketMessageReceivedListener
             */
            virtual void addListener(ISocketMessageReceivedListener *listener) = 0;

            /**
             * Remove a listener to stop it from being notified about received messages.
             *
             * @param *listener ISocketMessageReceivedListener
             */
            virtual void removeListener(ISocketMessageReceivedListener *listener) = 0;

            /**
             * Send a message contained with the buffer.
             *
             * @param *out const OutputBuffer containing the message to be sent
             *
             * A cadf::comms::SocketException will be thrown if an issue is encountered attempting to send the message.
             */
            virtual void send(const OutputBuffer *out) = 0;
    };

    /**
     * Data wrapper for a socket that facilitates the sending and receiving of data.
     */
    class TcpSocketDataHandler: public ISocketDataHandler, public cadf::thread::LoopingThread {
        public:
            /**
             * CTOR
             *
             * @param socketFd int the file descriptor of the socket
             * @param maxMessageSize size_t the max size of message that can be sent
             */
            TcpSocketDataHandler(int socketFd, size_t maxMessageSize);

            /**
             * DTOR
             */
            virtual ~TcpSocketDataHandler();

            /**
             * Add a listener to be notified when a message is received.
             *
             * @param *listener ISocketMessageReceivedListener
             */
            virtual void addListener(ISocketMessageReceivedListener *listener);

            /**
             * Remove a listener to stop it from being notified about received messages.
             *
             * @param *listener ISocketMessageReceivedListener
             */
            virtual void removeListener(ISocketMessageReceivedListener *listener);

            /**
             * Send a message contained with the buffer.
             *
             * @param *out const OutputBuffer containing the message to be sent
             *
             * A cadf::comms::MessageSendingException will be thrown if an issue is encountered attempting to send the message.
             */
            virtual void send(const OutputBuffer *out);

        private:
            /** Listeners to be notified when something is received */
            std::vector<ISocketMessageReceivedListener*> m_listeners;
            /** The socket from which to read */
            int m_socketFd;
            /** The maximum size of the data */
            size_t m_maxMessageSize;
            /** Buffer into which to read the data */
            char *m_messageBuffer;

            /**
             * Start reading message
             */
            virtual void start();

            /**
             * Stop reading messages
             */
            virtual void stop();

            /**
             * Called by the thread, waits for something to be delivered to the socket.
             */
            void execLoop();

            /**
             * Processes the received data.
             *
             * @param messageSize size_t the actual size of the data/message received
             */
            void processMessage(size_t messageSize);
    };
}

#endif /* CAMB_NETWORK_SOCKET_DATASOCKET_H_ */
