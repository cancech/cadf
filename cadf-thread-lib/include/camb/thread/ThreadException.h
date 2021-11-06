#ifndef CAMB_THREAD_THREADEXCEPTION_H_
#define CAMB_THREAD_THREADEXCEPTION_H_

#include <string>
#include <stdexcept>

namespace camb {

    /**
     * Exception that is to be thrown when initializing a thread
     */
    struct ThreadInitializationException: public std::runtime_error {
            ThreadInitializationException(const std::string &reason): std::runtime_error("Unable to perform thread initialization: " + reason) {
            }
    };
}

#endif /* CAMB_THREAD_THREADEXCEPTION_H_ */
