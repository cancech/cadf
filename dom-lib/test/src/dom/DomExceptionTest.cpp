#define BOOST_TEST_DYN_LINK
#ifdef STAND_ALONE
#   define BOOST_TEST_MODULE Main
#endif
#include <boost/test/unit_test.hpp>

#include "dom/DomException.h"

/**
 * Test suite for validating the DOM exceptions
 */
BOOST_AUTO_TEST_SUITE(DomException_Test_Suite)


    /**
     * Verify that the exception produces the correct message
     */
    BOOST_AUTO_TEST_CASE(DomParseExceptionMessageTest) {
        BOOST_CHECK_EQUAL("Error parsing: 'blah' expected at 123", cadf::dom::ParseException("blah", 123).what());
        BOOST_CHECK_EQUAL("Error parsing: 'abc123' expected at 234", cadf::dom::ParseException("abc123", 234).what());
        BOOST_CHECK_EQUAL("Error parsing: 'something or other' expected at -34", cadf::dom::ParseException("something or other", -34).what());
    }

    BOOST_AUTO_TEST_SUITE_END()
