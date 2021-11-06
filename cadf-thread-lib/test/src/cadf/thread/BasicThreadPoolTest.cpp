#define BOOST_TEST_DYN_LINK
#ifdef STAND_ALONE
#   define BOOST_TEST_MODULE Main
#endif
#include <boost/test/unit_test.hpp>
#include <chrono>
#include <thread>

#include "cadf/thread/BasicThreadPool.h"
#include "cadf/thread/ThreadException.h"

// Test helpers for the ThreadPoolTest
namespace BasicThreadPoolTest {
    /**
     * Helper to be used to count the number of times the method has been called
     */
    struct CallCounter {
            int numTimesCalled = 0;

            void callMe() {
                numTimesCalled++;
            }
    };
}

/**
 * Test suite for the unit testing of the BasicThreadPool
 */
BOOST_AUTO_TEST_SUITE(BasicThreadPool_Test_Suite)

/**
 * Verify that if an invalid number of threads is specified, an exception is thrown
 */
    BOOST_AUTO_TEST_CASE(ThreadPoolInvalidInitializationTest) {
        BOOST_REQUIRE_THROW(cadf::thread::BasicThreadPool(0), cadf::thread::ThreadInitializationException);
    }

    /**
     * Verify that the pool is not started if auto initialize is set to false
     */
    BOOST_AUTO_TEST_CASE(ThreadPoolNoAutoInitializationTest) {
        cadf::thread::BasicThreadPool p(1, false);
        BOOST_CHECK_EQUAL(false, p.isStarted());

        p.start();
        BOOST_CHECK_EQUAL(true, p.isStarted());
        p.stop();
        BOOST_CHECK_EQUAL(false, p.isStarted());
    }

    /**
     * Verify that can auto-start with a single thread
     */
    BOOST_AUTO_TEST_CASE(ThreadPoolAutoInitSingleThread) {
        BasicThreadPoolTest::CallCounter counter;
        cadf::thread::BasicThreadPool p(1);

        // With a single function scheduled
        p.schedule(std::bind(&BasicThreadPoolTest::CallCounter::callMe, &counter));
        BOOST_CHECK_EQUAL(0, counter.numTimesCalled);
        std::this_thread::sleep_for(std::chrono::milliseconds(10)); // 10 ms wait should be long enough for the thread to process
        BOOST_CHECK_EQUAL(1, counter.numTimesCalled);

        // With multiple functions scheduled
        p.schedule(std::bind(&BasicThreadPoolTest::CallCounter::callMe, &counter));
        p.schedule(std::bind(&BasicThreadPoolTest::CallCounter::callMe, &counter));
        p.schedule(std::bind(&BasicThreadPoolTest::CallCounter::callMe, &counter));
        BOOST_CHECK_EQUAL(1, counter.numTimesCalled);
        std::this_thread::sleep_for(std::chrono::milliseconds(10)); // 10 ms wait should be long enough for the threads to process
        BOOST_CHECK_EQUAL(4, counter.numTimesCalled);
    }

    /**
     * Verify that can auto-start with multiple threads
     */
    BOOST_AUTO_TEST_CASE(ThreadPoolAutoInitMultipleThreads) {
        BasicThreadPoolTest::CallCounter counter1;
        BasicThreadPoolTest::CallCounter counter2;
        BasicThreadPoolTest::CallCounter counter3;
        cadf::thread::BasicThreadPool p;

        BOOST_CHECK_EQUAL(0, counter1.numTimesCalled);
        BOOST_CHECK_EQUAL(0, counter2.numTimesCalled);
        BOOST_CHECK_EQUAL(0, counter3.numTimesCalled);
        p.schedule(std::bind(&BasicThreadPoolTest::CallCounter::callMe, &counter1));
        p.schedule(std::bind(&BasicThreadPoolTest::CallCounter::callMe, &counter2));
        p.schedule(std::bind(&BasicThreadPoolTest::CallCounter::callMe, &counter3));
        std::this_thread::sleep_for(std::chrono::milliseconds(10)); // 10 ms wait should be long enough for the thread to process
        BOOST_CHECK_EQUAL(1, counter1.numTimesCalled);
        BOOST_CHECK_EQUAL(1, counter2.numTimesCalled);
        BOOST_CHECK_EQUAL(1, counter3.numTimesCalled);
    }

    /**
     * Verify that functions scheduled prior to starting are executed on start across a single thread
     */
    BOOST_AUTO_TEST_CASE(ScheduleForExecutionWhenNotStartedSingleThread) {
        BasicThreadPoolTest::CallCounter counter1;
        BasicThreadPoolTest::CallCounter counter2;
        BasicThreadPoolTest::CallCounter counter3;
        cadf::thread::BasicThreadPool p(1, false);
        BOOST_CHECK_EQUAL(false, p.isStarted());

        p.schedule(std::bind(&BasicThreadPoolTest::CallCounter::callMe, &counter1));
        p.schedule(std::bind(&BasicThreadPoolTest::CallCounter::callMe, &counter2));
        p.schedule(std::bind(&BasicThreadPoolTest::CallCounter::callMe, &counter3));
        BOOST_CHECK_EQUAL(0, counter1.numTimesCalled);
        BOOST_CHECK_EQUAL(0, counter2.numTimesCalled);
        BOOST_CHECK_EQUAL(0, counter3.numTimesCalled);
        std::this_thread::sleep_for(std::chrono::milliseconds(10)); // 10 ms wait should be long enough for the thread to process
        // Thread pool is not started, so nothing is executed
        BOOST_CHECK_EQUAL(0, counter1.numTimesCalled);
        BOOST_CHECK_EQUAL(0, counter2.numTimesCalled);
        BOOST_CHECK_EQUAL(0, counter3.numTimesCalled);

        // Start the thread pool, and all buffered calls will execute
        p.start();
        std::this_thread::sleep_for(std::chrono::milliseconds(10)); // 10 ms wait should be long enough for the thread to process
        BOOST_CHECK_EQUAL(1, counter1.numTimesCalled);
        BOOST_CHECK_EQUAL(1, counter2.numTimesCalled);
        BOOST_CHECK_EQUAL(1, counter3.numTimesCalled);
    }

    /**
     * Verify that functions scheduled prior to starting are executed on start across multiple threads
     */
    BOOST_AUTO_TEST_CASE(ScheduleForExecutionWhenNotStartedMultipleThreads) {
        BasicThreadPoolTest::CallCounter counter1;
        BasicThreadPoolTest::CallCounter counter2;
        BasicThreadPoolTest::CallCounter counter3;
        cadf::thread::BasicThreadPool p(3, false);
        BOOST_CHECK_EQUAL(false, p.isStarted());

        p.schedule(std::bind(&BasicThreadPoolTest::CallCounter::callMe, &counter1));
        p.schedule(std::bind(&BasicThreadPoolTest::CallCounter::callMe, &counter2));
        p.schedule(std::bind(&BasicThreadPoolTest::CallCounter::callMe, &counter3));
        BOOST_CHECK_EQUAL(0, counter1.numTimesCalled);
        BOOST_CHECK_EQUAL(0, counter2.numTimesCalled);
        BOOST_CHECK_EQUAL(0, counter3.numTimesCalled);
        std::this_thread::sleep_for(std::chrono::milliseconds(10)); // 10 ms wait should be long enough for the thread to process
        // Thread pool is not started, so nothing is executed
        BOOST_CHECK_EQUAL(0, counter1.numTimesCalled);
        BOOST_CHECK_EQUAL(0, counter2.numTimesCalled);
        BOOST_CHECK_EQUAL(0, counter3.numTimesCalled);

        // Start the thread pool, and all buffered calls will execute
        p.start();
        std::this_thread::sleep_for(std::chrono::milliseconds(10)); // 10 ms wait should be long enough for the thread to process
        BOOST_CHECK_EQUAL(1, counter1.numTimesCalled);
        BOOST_CHECK_EQUAL(1, counter2.numTimesCalled);
        BOOST_CHECK_EQUAL(1, counter3.numTimesCalled);
    }

    BOOST_AUTO_TEST_SUITE_END()
