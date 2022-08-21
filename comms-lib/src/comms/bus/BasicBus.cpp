#include "comms/bus/BasicBus.h"
#include <algorithm>

namespace cadf::comms {

    /**
     * Send a message the message to the desired recipient(s).
     */
    void BasicBus::sendMessage(IBusConnection *sender, const MessagePacket *packet) {
        routeMessage(sender, packet);
    }

    /**
     * Send the message to the recipient
     */
    void BasicBus::sendMessage(IBusConnection *sender, IBusConnection *recipient, const MessagePacket *packet) {
        recipient->sendMessage(sender, packet);
    }
}
