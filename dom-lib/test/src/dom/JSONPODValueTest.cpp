#define BOOST_TEST_DYN_LINK
#ifdef STAND_ALONE
#   define BOOST_TEST_MODULE Main
#endif
#include <boost/test/unit_test.hpp>

#include "dom/JSONPODValue.h"

/**
 * Test suite for the JSONPODValue class
 */
BOOST_AUTO_TEST_SUITE(JSONPODValue_Test_Suite)

/**
 * Verify it can properly store and retrieve integers
 */
    BOOST_AUTO_TEST_CASE(IntegerValueTest) {
        cadf::dom::JSONPODValue value1("", 13579);
        BOOST_CHECK_EQUAL("13579", value1.valueToString());
        BOOST_CHECK_EQUAL(13579, value1.getValue<int>());

        cadf::dom::JSONPODValue value2("", 54321);
        BOOST_CHECK_EQUAL("54321", value2.valueToString());
        BOOST_CHECK_EQUAL(54321, value2.getValue<int>());
    }

    /**
     * Verify it can properly store and retrieve strings
     */
    BOOST_AUTO_TEST_CASE(StringValueTest) {
        std::string val1Str = "This is a string!";
        cadf::dom::JSONPODValue value1("", val1Str);
        BOOST_CHECK_EQUAL("\"This is a string!\"", value1.valueToString());
        BOOST_CHECK_EQUAL(val1Str, value1.getValue<std::string>());

        std::string val2Str = "AnotherStringForTheTest";
        cadf::dom::JSONPODValue value2("", val2Str);
        BOOST_CHECK_EQUAL("\"AnotherStringForTheTest\"", value2.valueToString());
        BOOST_CHECK_EQUAL(val2Str, value2.getValue<std::string>());
    }

    /**
     * Verify it can properly store and retrieve bools
     */
    BOOST_AUTO_TEST_CASE(BoolValueTest) {
        cadf::dom::JSONPODValue value1("", true);
        BOOST_CHECK_EQUAL("true", value1.valueToString());
        BOOST_CHECK_EQUAL(true, value1.getValue<bool>());

        cadf::dom::JSONPODValue value2("", false);
        BOOST_CHECK_EQUAL("false", value2.valueToString());
        BOOST_CHECK_EQUAL(false, value2.getValue<bool>());
    }

    /**
     * Verify it can properly store and retrieve chars
     */
    BOOST_AUTO_TEST_CASE(CharValueTest) {
        cadf::dom::JSONPODValue value1("", 'q');
        BOOST_CHECK_EQUAL("113", value1.valueToString());
        BOOST_CHECK_EQUAL('q', value1.getValue<char>());

        cadf::dom::JSONPODValue value2("", '&');
        BOOST_CHECK_EQUAL("38", value2.valueToString());
        BOOST_CHECK_EQUAL('&', value2.getValue<char>());
    }

    /**
     * Verify it can properly store and retrieve doubles
     */
    BOOST_AUTO_TEST_CASE(DoubleValueTest) {
        cadf::dom::JSONPODValue value1("", -5.09);
        BOOST_CHECK_EQUAL("-5.09", value1.valueToString());
        BOOST_CHECK_EQUAL(-5.09, value1.getValue<double>());

        cadf::dom::JSONPODValue value2("", 88.77);
        BOOST_CHECK_EQUAL("88.77", value2.valueToString());
        BOOST_CHECK_EQUAL(88.77, value2.getValue<double>());
    }

    BOOST_AUTO_TEST_SUITE_END()
