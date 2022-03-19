#include "thread/Thread.h"
#include "thread/ThreadException.h"
#include <functional>
#include <future>

namespace cadf::thread {

    bool ThreadInterruptConfigurator::m_isConfigured = false;

    /*
     * Configure the mechanism to allow for interrupting blocking calls in the thread.
     */
    void ThreadInterruptConfigurator::configure() {
        if (m_isConfigured)
            return;
        m_isConfigured = true;

        // Configure the signal
        siginterrupt(INTERRUPT_SIGNAL, true);
        std::signal(INTERRUPT_SIGNAL, ThreadInterruptConfigurator::processSignal);
    }

    /*
     * Required to ensure that the thread actually receives the signal and not the whole application
     */
    void ThreadInterruptConfigurator::processSignal(int) {
        // Intentionally left blank
    }

    /**
     * Create a new thread with the specified Task.
     */
    Thread::Thread(Task *executable): m_task(executable), m_alive(false) {
        ThreadInterruptConfigurator::configure();
    }

    /*
     * DTOR
     */
    Thread::~Thread() {
        stop();
    }

    /*
     * Start the thread, making sure to cleanup anything left over from a previous run as necessary
     */
    void Thread::start() {
        if (m_task == NULL)
            throw ThreadInitializationException("Cannot start a thread with a NULL task");
        else if (isAlive())
            throw ThreadInitializationException("Cannot start a running thread");
        else if (m_thread.joinable())
            m_thread.join();

        std::promise<void> threadStarted;
        std::future<void> waitForStart = threadStarted.get_future();
        m_thread = std::thread([&] {
            m_alive = true;
            threadStarted.set_value();
            m_task->exec();
            m_alive = false;
        });
        waitForStart.get();
    }

    /*
     * Stops the thread
     */
    void Thread::stop() {
        if (!isAlive()) {
            if (m_thread.joinable())
                m_thread.join();
            return;
        }

        m_task->scheduleStop();
        pthread_kill(m_thread.native_handle(), ThreadInterruptConfigurator::INTERRUPT_SIGNAL);
        std::raise(ThreadInterruptConfigurator::INTERRUPT_SIGNAL);
        if (m_thread.joinable())
            m_thread.join();
    }

    /*
     * Check if the thread is alive
     */
    bool Thread::isAlive() {
        return m_alive;
    }




    /**
     * Create a new one-shot thread with itself as the task
     */
    OneShotThread::OneShotThread() : Thread(this) {
    }





    /**
     * Create a new looping thread with itself as the task
     */
    LoopingThread::LoopingThread() : Thread(this) {
    }
}
