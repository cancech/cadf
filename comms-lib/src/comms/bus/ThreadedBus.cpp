#include "comms/bus/ThreadedBus.h"

namespace cadf::comms {
    /**
     * CTOR
     */
    ThreadedBus::ThreadedBus(cadf::thread::IThreadPool *pool) : m_threadPool(pool) {
    }

    /**
     * DTOR
     */
    ThreadedBus::~ThreadedBus() {
    }

    /**
     * Schedule the sending of the message.
     */
    void ThreadedBus::sendMessage(IBusConnection *sender, const MessagePacket *packet) {
        // Need to clone the packet as we have no control overly the packet life cycle from the caller. With the thread
        // the sending is now asynchronous from the caller, and so they could very easily cleanup the packet before
        // the thread is able to send it.
        m_threadPool->schedule(std::bind(&ThreadedBus::routeMessage, this, sender, packet->clone()));
    }

    /**
     * Pass the message to the recipient
     */
    void ThreadedBus::sendMessage(IBusConnection *sender, IBusConnection *recipient, const MessagePacket *packet) {
        recipient->sendMessage(sender, packet);
    }

    /**
     * Route the message and manage the allocated memory.
     */
    void ThreadedBus::routeMessage(IBusConnection *sender, const MessagePacket *packet) {
        AbstractBus::routeMessage(sender, packet);
        // Once the routing and sending of the packet is complete, need to clean up the packet clone.
        delete (packet);
    }
}
