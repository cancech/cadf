#define BOOST_TEST_DYN_LINK
#ifdef STAND_ALONE
#   define BOOST_TEST_MODULE Main
#endif
#include <boost/test/unit_test.hpp>

#include "comms/message/MessageException.h"

/**
 * Unit test for the Message Exceptions
 */
BOOST_AUTO_TEST_SUITE(MessageException_Test_Suite)

/**
 * Verify the instalid message exception is properly formated.
 */
    BOOST_AUTO_TEST_CASE(InvalidMessageExceptionMessageTest) {
        BOOST_CHECK_EQUAL("Invalid message type \"SOMETHING\": OTHER", cadf::comms::InvalidMessageTypeException("SOMETHING", "OTHER").what());
        BOOST_CHECK_EQUAL("Invalid message type \"REASON\": EXPLANATION OF EXCEPTION", cadf::comms::InvalidMessageTypeException("REASON", "EXPLANATION OF EXCEPTION").what());
        BOOST_CHECK_EQUAL("Invalid message type \"BLAHBLAHBLAH\": Fie, fih, foh, fum", cadf::comms::InvalidMessageTypeException("BLAHBLAHBLAH", "Fie, fih, foh, fum").what());
    }

    BOOST_AUTO_TEST_SUITE_END()
