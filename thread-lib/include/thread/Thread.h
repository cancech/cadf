#ifndef CAMB_THREAD_THREAD_H_
#define CAMB_THREAD_THREAD_H_

#include "thread/Task.h"
#include <thread>
#include <atomic>

namespace cadf::thread {

    /**
     * Helper that makes sure that the interruption is properly configured for a thread.
     */
    class ThreadInterruptConfigurator {
        public:
            /** The signal that is to be raised to interrupt the blocking behavior within a thread */
            static const int INTERRUPT_SIGNAL = SIGINT;

            /**
             * Configure the interruption signal.
             */
            static void configure();

        private:
            /** Flag for whether the configuration has already been performed */
            static bool m_isConfigured;

            /**
             * Signal handler for processing the raised signal. Note, that this does nothing as it is only present to ensure that the interrupt signal
             * is properly handled.
             *
             * @param int the raised signal (not used)
             */
            static void processSignal(int);
    };

    /**
     * Thread that will execute the logic in the specified executable in a separate thread. Also provides the necessary functionality in order to fully
     * manage the new thread.
     */
    class Thread  {
        public:
            /**
             * CTOR
             *
             * Creates a new thread in which the specified task will be executed.
             *
             * @param *task Task to be executed within the thread.
             */
            Thread(Task *task);

            /**
             * DTOR
             *
             * As part of destroying the thread, the Task running within will be stopped.
             */
            virtual ~Thread();

            /**
             * Start executing the task.
             */
            virtual void start();

            /**
             * Stop executing the task
             */
            virtual void stop();

            /**
             * Check if the thread is alive and the task is executing.
             *
             * @return bool true if the thread is alive
             */
            virtual bool isAlive();

        private:
            /** The task to execute within this thread */
            Task *m_task;
            /** Flag for whether or not the task is executing */
            std::atomic<bool> m_alive;
            /** The thread in which the task will actually execute */
            std::thread m_thread;
    };


    /**
     * Abstract thread where the thread object also acts as the Task. The child class must provide the actual logic, as well as the necessary mechanism
     * for stopping aborting the execution (as per Task).
     */
    class OneShotThread: public Thread, public Task {
        public:
            /**
             * CTOR
             *
             * Creates a new thread, where the thread itself is the executable.
             */
            OneShotThread();

            /**
             * DTOR
             */
            virtual ~OneShotThread() = default;

        protected:
            /**
             * The logic of the internal Task. By default does nothing, override with a child class to provide functionality.
             */
            virtual void exec() = 0;

            /**
             * The mechanism of scheduling the internal Task to end its execution. By default does nothing, override with a child class to provide the
             * appropriate stopping mechanism.
             */
            virtual void scheduleStop() = 0;

    };


    /**
     * Abstract thread where the thread object also acts as the Task. The thread supports a looping behavior, meaning that once started execLoop() will
     * continue to be called until the thread is stopped (as per LoopingTask).
     */
    class LoopingThread: public Thread, public LoopingTask {
        public:
            /**
             * CTOR
             *
             * Creates a new thread, where the thread itself is the executable.
             */
            LoopingThread();

            /**
             * DTOR
             */
            virtual ~LoopingThread() = default;

        protected:
            /**
             * Override with the details of what each iteration of the execution should perform.
             */
            virtual void execLoop() = 0;
    };
}

#endif /* CAMB_THREAD_THREAD_H_ */
