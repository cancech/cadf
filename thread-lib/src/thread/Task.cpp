#include <thread/Task.h>


namespace cadf::thread {
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
