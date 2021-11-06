#ifndef INCLUDE_TESTTASK_H_
#define INCLUDE_TESTTASK_H_

#include "cadf/thread/Task.h"
#include <unistd.h>

class TestNonBlockingTask: public cadf::thread::Task {
    public:
        TestNonBlockingTask();
        virtual ~TestNonBlockingTask() = default;
        virtual void exec();
        virtual void scheduleStop();

        int timesExecCalled();
        int timesScheduleStopCalled();

    private:
        int m_timesExecCalled;
        int m_timesScheduleStopCalled;
};

class TestBlockingTask: public TestNonBlockingTask {
    public:
        TestBlockingTask();
        virtual ~TestBlockingTask() = default;
        virtual void exec();
};

class TestNonBlockingLoopingTask: public cadf::thread::LoopingTask {
    public:
        TestNonBlockingLoopingTask();

        virtual void execLoop();
        int getTimesCalled();

    private:
        int m_timesCalled;
};

class TestBlockingLoopingTask: public cadf::thread::LoopingTask {
    public:
        TestBlockingLoopingTask();

        virtual void execLoop();

        bool wasCalled();

    private:
        bool m_called;
};


#endif /* INCLUDE_TESTTASK_H_ */
