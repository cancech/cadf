#ifndef CAMB_THREAD_TASK_H_
#define CAMB_THREAD_TASK_H_

#include <csignal>

namespace camb {

    /**
     * Task that is to be executed within a Thread. The logic of the task it be contained within the exec(), with scheduleStop() being used to allow
     * premature stop/abort. LoopingTask can also be used to help facilitate a repetitive action (i.e.: reading messages).
     *
     * Note: for any blocking behavior an interrupt signal (SIGINT) will be raised when stopping the task. Ensure that any/all blocking behavior respect
     * this signal to properly ensure that the task can terminate on thread stop.
     */
    class Task {
        public:
            /**
             * DTOR
             */
            virtual ~Task() = default;

            /**
             * Override with the details of what the task should perform
             */
            virtual void exec() = 0;

            /**
             * Override with the necessary mechanism to allow for a premature end of the task.
             */
            virtual void scheduleStop() = 0;
    };

    /**
     * A convenience task to facilitate a looping/repetitive task. For example the continuous reading of new messages as they arrive. Exec will handle
     * the repetitiveness and scheduleStop will break the loop. The actual logic is to per added into the execLoop().
     */
    class LoopingTask: public Task {
        public:
            /**
             * CTOR
             */
            LoopingTask();

            /**
             * DTOR
             */
            virtual ~LoopingTask();

            /**
             * Perform the looping behavior of the task. Each iteration will call execLoop(), and continue iterating until scheduleStop() is called.
             */
            virtual void exec();

            /**
             * Override with the details of what each iteration of the task should perform.
             */
            virtual void execLoop() = 0;

            /**
             * Stops the looping behavior, will not break out of the execLoop. Any blocking behavior within must be allow itself to be interrupted
             * with the interrupt signal (SIGINT)
             */
            virtual void scheduleStop();

        private:
            /** Flag for whether or not the end of the execution loop is desired */
            bool m_quitting;
    };

}

#endif /* CAMB_THREAD_TASK_H_ */
