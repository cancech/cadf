#define BOOST_TEST_DYN_LINK
#ifdef STAND_ALONE
#   define BOOST_TEST_MODULE Main
#endif
#include <boost/test/unit_test.hpp>

#include "comms/connection/ConnectionException.h"

/**
 * Unit test for the Connection Exceptions
 */
BOOST_AUTO_TEST_SUITE(ConnectionException_Test_Suite)

/**
 * Verify receiving a message without a registered listener does nothing
 */
    BOOST_AUTO_TEST_CASE(BusExceptionMessageTest) {
        BOOST_CHECK_EQUAL("Unable to perform Bus Operation: SOMETHING", cadf::comms::BusException("SOMETHING").what());
        BOOST_CHECK_EQUAL("Unable to perform Bus Operation: OR", cadf::comms::BusException("OR").what());
        BOOST_CHECK_EQUAL("Unable to perform Bus Operation: BLAHBLAHBLAH123!@#", cadf::comms::BusException("BLAHBLAHBLAH123!@#").what());
    }

    /**
     * Verify receiving a message without a registered listener does nothing
     */
    BOOST_AUTO_TEST_CASE(MessageSendingExceptionTest) {
        BOOST_CHECK_EQUAL("Unable to send message \"TYPE\": REASON", cadf::comms::MessageSendingException("TYPE", "REASON").what());
        BOOST_CHECK_EQUAL("Unable to send message \"ANOTHER MSG\": AN explanation", cadf::comms::MessageSendingException("ANOTHER MSG", "AN explanation").what());
        BOOST_CHECK_EQUAL("Unable to send message \"blah\": 1234567890!@@#$%^", cadf::comms::MessageSendingException("blah", "1234567890!@@#$%^").what());
    }

    BOOST_AUTO_TEST_SUITE_END()
