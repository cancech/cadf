#define BOOST_TEST_DYN_LINK
#ifdef STAND_ALONE
#   define BOOST_TEST_MODULE Main
#endif
#include <boost/test/unit_test.hpp>
#include <thread>

#include "thread/Thread.h"
#include "thread/ThreadException.h"

#include "TestTask.h"

namespace ThreadTest {

    class TestOneShotThread: public cadf::thread::OneShotThread {
        public:
            void verify(int expectExecCalled, int expectedScheduleStopCalled) {
                BOOST_CHECK_EQUAL(expectExecCalled, m_timesExecCalled);
                BOOST_CHECK_EQUAL(expectedScheduleStopCalled, m_timesScheduleStopCalled);
            }

        protected:
            virtual void exec() {
                m_timesExecCalled++;
                std::this_thread::sleep_for(std::chrono::milliseconds(2));
            }

            virtual void scheduleStop() {
                m_timesScheduleStopCalled++;
            }

        private:
            int m_timesExecCalled = 0;
            int m_timesScheduleStopCalled = 0;
    };

    class TestLoopingThread: public cadf::thread::LoopingThread {
        public:
            int m_timesLoopCalled = 0;

        protected:
            virtual void execLoop() {
                m_timesLoopCalled++;
            }
    };

    struct TimedTest {
            TimedTest() {
                m_start = std::chrono::high_resolution_clock::now();
            }

            ~TimedTest() {
                auto endTime = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::seconds>(endTime - m_start);
                BOOST_CHECK_MESSAGE(duration.count() < 1, "Test took too long to complete");
            }

        private:
            std::chrono::system_clock::time_point m_start;
    };

}

/**
 * Unit test for Thread
 */
BOOST_AUTO_TEST_SUITE(Thread_Test_Suite)

/**
 * Verify that a single-shot (non-looping) thread performs as expected.
 */
    BOOST_FIXTURE_TEST_CASE(NonBlockingExecutableTest, ThreadTest::TimedTest) {
        TestNonBlockingTask executable;
        cadf::thread::Thread thread(&executable);

        // Verify the initial state
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        BOOST_CHECK_EQUAL(0, executable.timesExecCalled());
        BOOST_CHECK_EQUAL(0, executable.timesScheduleStopCalled());
        BOOST_CHECK(!thread.isAlive());

        // Start the thread and make sure that the execution takes place
        thread.start();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        BOOST_CHECK_EQUAL(1, executable.timesExecCalled());
        BOOST_CHECK_EQUAL(0, executable.timesScheduleStopCalled());
        BOOST_CHECK(!thread.isAlive());

        // Make sure that stop does nothing
        thread.stop();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        BOOST_CHECK_EQUAL(1, executable.timesExecCalled());
        BOOST_CHECK_EQUAL(0, executable.timesScheduleStopCalled());
        BOOST_CHECK(!thread.isAlive());
    }

    /**
     * Verify that a single-shot (non-looping) thread performs as expected.
     */
    BOOST_FIXTURE_TEST_CASE(BlockingExecutableTest, ThreadTest::TimedTest) {
        TestBlockingTask executable;
        cadf::thread::Thread thread(&executable);

        // Verify the initial state
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        BOOST_CHECK_EQUAL(0, executable.timesExecCalled());
        BOOST_CHECK_EQUAL(0, executable.timesScheduleStopCalled());
        BOOST_CHECK(!thread.isAlive());

        // Start the thread and make sure that the execution takes place
        thread.start();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        BOOST_CHECK_EQUAL(1, executable.timesExecCalled());
        BOOST_CHECK_EQUAL(0, executable.timesScheduleStopCalled());
        BOOST_CHECK(thread.isAlive());

        // Make the thread stops
        thread.stop();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        BOOST_CHECK_EQUAL(1, executable.timesExecCalled());
        BOOST_CHECK_EQUAL(1, executable.timesScheduleStopCalled());
        BOOST_CHECK(!thread.isAlive());
    }

    /**
     * Verify that the thread performs as expected when executing a non-blocking looping task
     */
    BOOST_FIXTURE_TEST_CASE(NonBlockingLoopingExecutableTest, ThreadTest::TimedTest) {
        TestNonBlockingLoopingTask executable;
        cadf::thread::Thread thread(&executable);

        // Verify the initial state
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        BOOST_CHECK_EQUAL(0, executable.getTimesCalled());
        BOOST_CHECK(!thread.isAlive());

        // Start the thread and make sure that execution is happening
        thread.start();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        BOOST_CHECK(thread.isAlive());
        BOOST_CHECK(executable.getTimesCalled() > 0);
        int currTimeCalled = executable.getTimesCalled();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        BOOST_CHECK(thread.isAlive());
        BOOST_CHECK(executable.getTimesCalled() > currTimeCalled);

        // Stop and make sure that it stops
        thread.stop();
        BOOST_CHECK(!thread.isAlive());
        currTimeCalled = executable.getTimesCalled();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        BOOST_CHECK(!thread.isAlive());
        BOOST_CHECK_EQUAL(currTimeCalled, executable.getTimesCalled());
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        BOOST_CHECK(!thread.isAlive());
        BOOST_CHECK_EQUAL(currTimeCalled, executable.getTimesCalled());
    }

    /**
     * Verify that the thread performs as expected when executing a blocking looping task
     */
    BOOST_FIXTURE_TEST_CASE(BlockingLoopingExecutableTest, ThreadTest::TimedTest) {
        TestBlockingLoopingTask executable;
        cadf::thread::Thread thread(&executable);

        // Verify the initial state
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        BOOST_CHECK(!executable.wasCalled());
        BOOST_CHECK(!thread.isAlive());

        // Start the thread and make sure that execution is happening
        thread.start();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        BOOST_CHECK(thread.isAlive());
        BOOST_CHECK(executable.wasCalled());

        // Stop and make sure that it stops
        thread.stop();
        BOOST_CHECK(!thread.isAlive());
    }

    /**
     * Verify that nothing happens when stopping a thread that hasn't been started yet.
     */
    BOOST_FIXTURE_TEST_CASE(StopUnstartThreadTest, ThreadTest::TimedTest) {
        TestNonBlockingTask executable;
        cadf::thread::Thread thread(&executable);

        // Verify the initial state
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        BOOST_CHECK_EQUAL(0, executable.timesExecCalled());
        BOOST_CHECK_EQUAL(0, executable.timesScheduleStopCalled());
        BOOST_CHECK(!thread.isAlive());

        // Make sure that stop does nothing
        thread.stop();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        BOOST_CHECK_EQUAL(0, executable.timesExecCalled());
        BOOST_CHECK_EQUAL(0, executable.timesScheduleStopCalled());
        BOOST_CHECK(!thread.isAlive());
    }

    /**
     * Verify that nothing happens when starting a thread that's already started
     */
    BOOST_FIXTURE_TEST_CASE(StartStartedThreadTest, ThreadTest::TimedTest) {
        TestBlockingTask executable;
        cadf::thread::Thread thread(&executable);

        // Verify the initial state
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        BOOST_CHECK_EQUAL(0, executable.timesExecCalled());
        BOOST_CHECK_EQUAL(0, executable.timesScheduleStopCalled());
        BOOST_CHECK(!thread.isAlive());

        // Start the thread
        thread.start();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        BOOST_CHECK_EQUAL(1, executable.timesExecCalled());
        BOOST_CHECK_EQUAL(0, executable.timesScheduleStopCalled());
        BOOST_CHECK(thread.isAlive());

        // Start it again
        BOOST_CHECK_THROW(thread.start(), cadf::thread::ThreadInitializationException);
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        BOOST_CHECK_EQUAL(1, executable.timesExecCalled());
        BOOST_CHECK_EQUAL(0, executable.timesScheduleStopCalled());
        BOOST_CHECK(thread.isAlive());

        // Stop the thread
        thread.stop();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        BOOST_CHECK_EQUAL(1, executable.timesExecCalled());
        BOOST_CHECK_EQUAL(1, executable.timesScheduleStopCalled());
        BOOST_CHECK(!thread.isAlive());
    }

    /**
     * Verify that nothing happens when starting a thread that's already started
     */
    BOOST_FIXTURE_TEST_CASE(StartThreadNotYetJoinedTest, ThreadTest::TimedTest) {
        TestNonBlockingTask executable;
        cadf::thread::Thread thread(&executable);

        // Verify the initial state
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        BOOST_CHECK_EQUAL(0, executable.timesExecCalled());
        BOOST_CHECK_EQUAL(0, executable.timesScheduleStopCalled());
        BOOST_CHECK(!thread.isAlive());

        // Start the thread
        thread.start();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        BOOST_CHECK_EQUAL(1, executable.timesExecCalled());
        BOOST_CHECK_EQUAL(0, executable.timesScheduleStopCalled());
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        BOOST_CHECK(!thread.isAlive());

        // Start it again
        thread.start();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        BOOST_CHECK_EQUAL(2, executable.timesExecCalled());
        BOOST_CHECK_EQUAL(0, executable.timesScheduleStopCalled());
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        BOOST_CHECK(!thread.isAlive());

        // Stop the thread
        thread.stop();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        BOOST_CHECK_EQUAL(2, executable.timesExecCalled());
        BOOST_CHECK_EQUAL(0, executable.timesScheduleStopCalled());
        BOOST_CHECK(!thread.isAlive());
    }

    /**
     * Verify the destructor terminates the thread properly/
     */
    BOOST_FIXTURE_TEST_CASE(DestructorStopsThreadTest, ThreadTest::TimedTest) {
        TestBlockingTask executable;
        cadf::thread::Thread *thread = new cadf::thread::Thread(&executable);

        // Verify the initial state
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        BOOST_CHECK_EQUAL(0, executable.timesExecCalled());
        BOOST_CHECK_EQUAL(0, executable.timesScheduleStopCalled());
        BOOST_CHECK(!thread->isAlive());

        // Start the thread and make sure that the execution takes place
        thread->start();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        BOOST_CHECK_EQUAL(1, executable.timesExecCalled());
        BOOST_CHECK_EQUAL(0, executable.timesScheduleStopCalled());
        BOOST_CHECK(thread->isAlive());

        // Make sure that deleting the thread also stops it
        delete (thread);
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        BOOST_CHECK_EQUAL(1, executable.timesExecCalled());
        BOOST_CHECK_EQUAL(1, executable.timesScheduleStopCalled());
    }

    /**
     * Verify an exception is throw when starting a thread with a NULL task.
     */
    BOOST_FIXTURE_TEST_CASE(NullTaskTest, ThreadTest::TimedTest) {
        cadf::thread::Thread thread(NULL);
        BOOST_CHECK(!thread.isAlive());
        BOOST_CHECK_THROW(thread.start(), cadf::thread::ThreadInitializationException);
        BOOST_CHECK(!thread.isAlive());
    }

    /**
     * Verify that a oneshot task will properly terminate itself once it completes
     */
    BOOST_FIXTURE_TEST_CASE(OneShotRunsCourseTest, ThreadTest::TimedTest) {
        ThreadTest::TestOneShotThread thread;
        BOOST_CHECK(!thread.isAlive());
        thread.start();
        BOOST_CHECK(thread.isAlive());
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        thread.verify(1, 0);
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
        BOOST_CHECK(!thread.isAlive());
        thread.verify(1, 0);
        thread.stop();
        thread.verify(1, 0);
    }

    /**
     * Verify that a oneshot task will properly stop when stopped
     */
    BOOST_FIXTURE_TEST_CASE(OneShotStopThreadTest, ThreadTest::TimedTest) {
        ThreadTest::TestOneShotThread thread;
        BOOST_CHECK(!thread.isAlive());
        thread.start();
        BOOST_CHECK(thread.isAlive());
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        thread.verify(1, 0);
        thread.stop();
        BOOST_CHECK(!thread.isAlive());
        thread.verify(1, 1);
    }

    /**
     * Verify that a looping thread can be properly started and stopped
     */
    BOOST_FIXTURE_TEST_CASE(LoopingThreaddTest, ThreadTest::TimedTest) {
        ThreadTest::TestLoopingThread thread;
        BOOST_CHECK(!thread.isAlive());
        thread.start();
        BOOST_CHECK(thread.isAlive());

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        int timesCurrentlyCalled = thread.m_timesLoopCalled;
        BOOST_CHECK(0 < timesCurrentlyCalled);
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        BOOST_CHECK(timesCurrentlyCalled < thread.m_timesLoopCalled);

        thread.stop();
        BOOST_CHECK(!thread.isAlive());
    }

    BOOST_AUTO_TEST_SUITE_END()
