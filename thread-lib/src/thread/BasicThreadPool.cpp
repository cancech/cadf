#include "thread/BasicThreadPool.h"
#include "thread/ThreadException.h"

#include <sstream>

namespace cadf::thread {

    /**
     * CTOR
     */
    BasicThreadPool::BasicThreadPool(unsigned int numThreads, bool autoStart) : m_started(false), m_terminating(false), m_numOfThreads(numThreads) {
        // Ensure that a valid number of threads are specified
        if (m_numOfThreads <= 0)
            throw ThreadInitializationException("At least one thread must be added to the ThreadPool");

        // Start the pool if desired
        if (autoStart)
            start();
    }

    /**
     * DTOR
     */
    BasicThreadPool::~BasicThreadPool() {
        stop();
    }

    /**
     * Start the pool
     */
    void BasicThreadPool::start() {
        std::unique_lock<std::mutex> lock(m_threadPoolMutex);
        if (m_started)
            return;

        // Create the threads
        for (unsigned int i = 0; i < m_numOfThreads; i++)
            m_threads.push_back(std::thread(std::bind(&BasicThreadPool::waitAndProcess, this)));
        m_started = true;
    }

    /**
     * Stop the pool
     */
    void BasicThreadPool::stop() {
        std::unique_lock<std::mutex> lock(m_threadPoolMutex);
        if (!m_started)
            return;

        // Terminate the threads
        m_terminating = true;
        m_bufferCondition.notify_all();

        for (std::thread &t : m_threads)
            t.join();

        // Purge them
        m_threads.clear();
        m_started = false;
    }

    /**
     * Check if the pool is started
     */
    bool BasicThreadPool::isStarted() {
        std::unique_lock<std::mutex> lock(m_threadPoolMutex);
        return m_started;
    }

    /**
     * Schedule function for execution
     */
    void BasicThreadPool::schedule(std::function<void()> func) {
        {
            std::unique_lock<std::mutex> lock(m_bufferMutex);
            m_buffer.push(func);
        }
        m_bufferCondition.notify_one();
    }

    /**
     * Wait for a function to be buffered and then execute it
     */
    void BasicThreadPool::waitAndProcess() {
        while (true) {
            std::function<void()> func;

            {
                // Wait for something to appear on the buffer (or termination)
                std::unique_lock<std::mutex> lock(m_bufferMutex);
                m_bufferCondition.wait(lock, [=] {
                    return !m_buffer.empty() || m_terminating;
                });

                // If terminating, then stop trying to process buffers
                if (m_terminating)
                    break;

                // Grab the next waiting function
                func = m_buffer.front();
                m_buffer.pop();
            }

            // Now outside of the lock, process the function
            func();
        }
    }
}
