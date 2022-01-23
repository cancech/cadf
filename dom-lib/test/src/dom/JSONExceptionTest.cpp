#define BOOST_TEST_DYN_LINK
#ifdef STAND_ALONE
#   define BOOST_TEST_MODULE Main
#endif
#include <boost/test/unit_test.hpp>

#include "dom/JSONException.h"

/**
 * Test suite for validating the JSON exceptions
 */
BOOST_AUTO_TEST_SUITE(JSONException_Test_Suite)

/**
 * Verify that the exception produces the correct message
 */
    BOOST_AUTO_TEST_CASE(JSONTreeExceptionMessageTest) {
        BOOST_CHECK_EQUAL("Error parsing the JSON Tree: SOME MESSAGE", cadf::dom::JSONTreeException("SOME MESSAGE").what());
        BOOST_CHECK_EQUAL("Error parsing the JSON Tree: MessageABC123", cadf::dom::JSONTreeException("MessageABC123").what());
        BOOST_CHECK_EQUAL("Error parsing the JSON Tree: skfnhslknsdfglkn", cadf::dom::JSONTreeException("skfnhslknsdfglkn").what());
    }

    /**
     * Verify that the exception produces the correct message
     */
    BOOST_AUTO_TEST_CASE(JSONParseExceptionMessageTest) {
        BOOST_CHECK_EQUAL("Error parsing JSON: 'blah' expected at 123", cadf::dom::JSONParseException("blah", 123).what());
        BOOST_CHECK_EQUAL("Error parsing JSON: 'abc123' expected at 234", cadf::dom::JSONParseException("abc123", 234).what());
        BOOST_CHECK_EQUAL("Error parsing JSON: 'something or other' expected at -34", cadf::dom::JSONParseException("something or other", -34).what());
    }

    BOOST_AUTO_TEST_SUITE_END()
