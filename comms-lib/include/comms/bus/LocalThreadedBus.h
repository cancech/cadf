#ifndef CAMB_BUS_LOCALTHREADEDBUS_H_
#define CAMB_BUS_LOCALTHREADEDBUS_H_

#include <thread/ThreadPool.h>
#include "comms/bus/Bus.h"

namespace cadf::comms {

    /**
     * A local bus that uses a thread pool to help process the messages faster (and break the execution dependency between the sending thread
     * and the receiving thread).
     */
    class LocalThreadedBus: public AbstractBus {

        public:
            /**
             * CTOR
             *
             * Creates the bus with the provided thread pool.
             *
             * @param *pool IThreadPool pointer to the thread pool that is to be used
             */
            LocalThreadedBus(cadf::thread::IThreadPool *pool);

            /**
             * DTOR
             */
            virtual ~LocalThreadedBus();

            /**
             * Sends the message from the sender to the rest of the bus, as per the routing information in the packet.
             *
             * @param *sender IBusConnection who is sending the message
             * @param *packet MessagePacket that is to be sent
             */
            virtual void sendMessage(IBusConnection *sender, const MessagePacket *packet);

        protected:
            /**
             * Route the message to the appropriate recipient(s).
             *
             * @param *sender IBusConnection who is sending the message
             * @param *packet MessagePacket that is to be sent
             */
            virtual void routeMessage(IBusConnection *sender, const MessagePacket *packet);

        private:
            /** The thread pool that provides the threads for sending */
            cadf::thread::IThreadPool *m_threadPool;

            /**
             * Sends the message to the recipient.
             *
             * @param *sender IBusConnection who is sending the message
             * @param *recipient IBusConnection who is to receive the message
             * @param *packet MessagePacket that is to be sent
             */
            void sendMessage(IBusConnection *sender, IBusConnection *recipient, const MessagePacket *packet);
    };

}

#endif /* CAMB_BUS_LOCALTHREADEDBUS_H_ */
