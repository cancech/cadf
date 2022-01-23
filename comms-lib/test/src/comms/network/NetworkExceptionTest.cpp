#define BOOST_TEST_DYN_LINK
#ifdef STAND_ALONE
#   define BOOST_TEST_MODULE Main
#endif
#include <boost/test/unit_test.hpp>

#include "comms/network/NetworkException.h"

/**
 * Unit test for the Network Exceptions
 */
BOOST_AUTO_TEST_SUITE(NetworkException_Test_Suite)

/**
 * Verify that the buffer overflow exception is properly formatted.
 */
    BOOST_AUTO_TEST_CASE(BufferOverflowExceptionTest) {
        BOOST_CHECK_EQUAL("Buffer overflow detected", cadf::comms::BufferOverflowException().what());
    }

    /**
     * Verify that the protocol exception is properly formatted.
     */
    BOOST_AUTO_TEST_CASE(ProtocolExceptionTest) {
        BOOST_CHECK_EQUAL("Error processing the protocol PROTOCOL: REASON", cadf::comms::ProtocolException("PROTOCOL", "REASON").what());
        BOOST_CHECK_EQUAL("Error processing the protocol abc123: Enter some text here", cadf::comms::ProtocolException("abc123", "Enter some text here").what());
    }

    BOOST_AUTO_TEST_SUITE_END()
