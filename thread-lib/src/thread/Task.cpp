#include "thread/Task.h"
#include <unistd.h>


namespace cadf::thread {

    /*
     * Sleep for specified number of seconds
     */
    void Task::ssleep(unsigned int secs) {
        ::sleep(secs);
    }

    /*
     * Sleep for specified number of milliseconds
     */
    void Task::msleep(unsigned int millisecs) {
        usleep(1000 * millisecs);
    }

    /*
     * Sleep for specified number of microseconds
     */
    void Task::usleep(unsigned int microsecs) {
        ::usleep(microsecs);
    }

    /*
     * CTOR
     */
    LoopingTask::LoopingTask(): m_quitting(true) {

    }

    /*
     * DTOR
     */
    LoopingTask::~LoopingTask() {
        m_quitting = true;
    }

    /*
     * Mark the end of the exec loop iteration
     */
    void LoopingTask::scheduleStop() {
        m_quitting = true;
    }

    /*
     * Continue executing the execLoop() until scheduleStop()
     */
    void LoopingTask::exec() {
        m_quitting = false;
        while (!m_quitting)
            execLoop();
    }
}
