
#define BOOST_TEST_DYN_LINK
#ifdef STAND_ALONE
#   define BOOST_TEST_MODULE Main
#endif
#include <boost/test/unit_test.hpp>
#include <thread>

#include <cadf/thread/Task.h>

#include <TestTask.h>

/**
 * Unit test for Looping Executables
 */
BOOST_AUTO_TEST_SUITE(LoopingTask_Test_Suite)

    /**
     * Verify that a non-blocking executable is repeatedly called and stops being called when the executable is stopped
     */
    BOOST_AUTO_TEST_CASE(TestNonBlockingExecutable) {
        // Verify the initial state
        TestNonBlockingLoopingTask executable;
        BOOST_CHECK_EQUAL(0, executable.getTimesCalled());

        // Start execution in a separate thread
        std::thread thread(std::bind(&TestNonBlockingLoopingTask::exec, &executable));

        // Make sure that the thread is running and execution is happening
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        BOOST_CHECK(executable.getTimesCalled() > 0);
        int currTimeCalled = executable.getTimesCalled();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        BOOST_CHECK(executable.getTimesCalled() > currTimeCalled);

        // Stop and make sure that it stops
        executable.scheduleStop();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        currTimeCalled = executable.getTimesCalled();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        BOOST_CHECK_EQUAL(currTimeCalled, executable.getTimesCalled());
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        BOOST_CHECK_EQUAL(currTimeCalled, executable.getTimesCalled());

        thread.join();
    }

    BOOST_AUTO_TEST_SUITE_END()
