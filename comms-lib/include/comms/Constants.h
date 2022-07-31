#ifndef CAMB_CONSTANTS_H_
#define CAMB_CONSTANTS_H_

namespace cadf::comms {

    struct ConnectionConstants {
        /** Value to use to signify that destination is broadcast (send to all available for that field) */
        const static int BROADCAST = -1;
    };

    struct MessageConstants {
            /** Value to use to signify that the serialization buffer size should be automatically derived */
            const static size_t AUTO_SIZE = 0;
    };
}

#endif /* CAMB_CONSTANTS_H_ */
