#define BOOST_TEST_DYN_LINK
#ifdef STAND_ALONE
#   define BOOST_TEST_MODULE Main
#endif
#include <boost/test/unit_test.hpp>

#include "comms/network/socket/SocketException.h"

/**
 * Test suite for the ServerSocket
 */
BOOST_AUTO_TEST_SUITE(SocketException_Test_Suite)

    BOOST_AUTO_TEST_CASE(MessageTest) {
        BOOST_CHECK_EQUAL("message", cadf::comms::SocketException("message").what());
        BOOST_CHECK_EQUAL("abc123", cadf::comms::SocketException("abc123").what());
        BOOST_CHECK_EQUAL("This is the exception", cadf::comms::SocketException("This is the exception").what());
    }

BOOST_AUTO_TEST_SUITE_END()
