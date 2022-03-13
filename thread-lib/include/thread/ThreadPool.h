#ifndef CAMB_THREAD_THREADPOOL_H_
#define CAMB_THREAD_THREADPOOL_H_

#include <vector>
#include <queue>
#include <functional>
#include <mutex>
#include <condition_variable>

namespace cadf::thread {

    /**
     * Interface for the ThreadPool that the Threaded aspects of the CADF will use.
     */
    class IThreadPool {
        public:
            /**
             * DTOR
             */
            virtual ~IThreadPool() = default;

            /**
             * Schedules a std::function for execution. The function cannot take any parameters, and it will be placed
             * in a buffer until an available thread is able to proceed with its execution.
             *
             * @param func std::function<void()> that is to be scheduled for execution within the next available thread.
             */
            virtual void schedule(std::function<void()> func) = 0;
    };
}

#endif /* CAMB_THREAD_THREADPOOL_H_ */
