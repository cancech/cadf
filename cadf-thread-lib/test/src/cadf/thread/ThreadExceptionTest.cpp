#define BOOST_TEST_DYN_LINK
#ifdef STAND_ALONE
#   define BOOST_TEST_MODULE Main
#endif
#include <boost/test/unit_test.hpp>

#include "cadf/thread/ThreadException.h"

/**
 * Unit test for the Thread Exceptions
 */
BOOST_AUTO_TEST_SUITE(ThreadException_Test_Suite)
/**
 * Verify receiving a message without a registered listener does nothing
 */
    BOOST_AUTO_TEST_CASE(ThreadInitializationExceptionTest) {
        BOOST_CHECK_EQUAL("Unable to perform thread initialization: SOMETHING", cadf::thread::ThreadInitializationException("SOMETHING").what());
        BOOST_CHECK_EQUAL("Unable to perform thread initialization: OR OTHER", cadf::thread::ThreadInitializationException("OR OTHER").what());
        BOOST_CHECK_EQUAL("Unable to perform thread initialization: This is some error message", cadf::thread::ThreadInitializationException("This is some error message").what());
    }

    BOOST_AUTO_TEST_SUITE_END()
