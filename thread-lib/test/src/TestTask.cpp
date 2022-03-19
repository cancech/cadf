#include "TestTask.h"

TestNonBlockingTask::TestNonBlockingTask(): m_timesExecCalled(0), m_timesScheduleStopCalled(0) {
}

void TestNonBlockingTask::exec() {
    m_timesExecCalled++;
}

void TestNonBlockingTask::scheduleStop() {
    m_timesScheduleStopCalled++;
}

int TestNonBlockingTask::timesExecCalled() {
    return m_timesExecCalled;
}

int TestNonBlockingTask::timesScheduleStopCalled() {
    return m_timesScheduleStopCalled;
}


TestBlockingTask::TestBlockingTask(): TestNonBlockingTask() {
}

void TestBlockingTask::exec() {
    TestNonBlockingTask::exec();
    ssleep(10);
}


TestNonBlockingLoopingTask::TestNonBlockingLoopingTask(): m_timesCalled(0) {}

void TestNonBlockingLoopingTask::execLoop() {
    m_timesCalled++;
}

int TestNonBlockingLoopingTask::getTimesCalled() {
    return m_timesCalled;
}

TestBlockingLoopingTask::TestBlockingLoopingTask(): m_called(false) {}

void TestBlockingLoopingTask::execLoop() {
    m_called = true;
    ssleep(10);
}

bool TestBlockingLoopingTask::wasCalled() {
    return m_called;
}
