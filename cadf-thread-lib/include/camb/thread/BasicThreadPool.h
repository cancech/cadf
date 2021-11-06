#ifndef CAMB_THREAD_BASICTHREADPOOL_H_
#define CAMB_THREAD_BASICTHREADPOOL_H_

#include "camb/thread/ThreadPool.h"

#include <thread>

namespace camb {
    /**
     * Concrete (and basic) thread pool implementation that implements the IThreadPool interface
     */
    class BasicThreadPool: public IThreadPool {
        public:
            /**
             * CTOR
             *
             * Creates a thread pool with a number of available threads as specified by the user. The thread pool will
             * start by default, this this behavior can be controlled by the client code. Note, that specifying an invalid
             * number of threads (i.e.: 0) will generate an exception.
             *
             * @param numThreads int the number of threads that are to be made available in the thread pool (defaults to std::thread::hardware_concurrency())
             * @param autoStart bool to indicate whether the thread pool should be started on initialization (defaults to true)
             */
            BasicThreadPool(unsigned int numThreads = std::thread::hardware_concurrency(), bool autoStart = true);

            /**
             * DTOR
             */
            virtual ~BasicThreadPool();

            /**
             * Starts the thread pool, creates the assigned number of threads and prepares for the scheduling of executions. Does nothing if already started.
             */
            virtual void start();

            /**
             * Stops the thread pool and purges all existing threads. This will wait until all threads terminate their executions and properly join.
             * Does nothing if already stopped.
             */
            virtual void stop();

            /**
             * Check if the thread pool is currently started
             */
            virtual bool isStarted();

            /**
             * Schedules a function for execution in the next available thread.
             *
             * @param func std::function<void()> that is to be scheduled for execution
             */
            virtual void schedule(std::function<void()> func);

        private:
            /** Flag for whether or not the pool has been started */
            bool m_started;
            /** Flag for whether or not the pool is in the process of terminating */
            bool m_terminating;
            /** The number of threads that are to be created and managed */
            unsigned int m_numOfThreads;
            /** Vector of all created threads */
            std::vector<std::thread> m_threads;
            /** Mutex to ensure thread safety when managing the thread pool */
            std::mutex m_threadPoolMutex;
            /** Queue of functions that are buffered for execution */
            std::queue<std::function<void()>> m_buffer;
            /** Mutex to ensure thread safety when accessing the buffer */
            std::mutex m_bufferMutex;
            /** Condition which is used to wake up threads waiting on something to be placed on the buffer */
            std::condition_variable m_bufferCondition;

            /**
             * The logic for each thread in the pool. It will wait for a function to be placed on the buffer
             * for execution and then process it.
             */
            void waitAndProcess();
    };
}

#endif /* CAMB_THREAD_BASICTHREADPOOL_H_ */
