#define BOOST_TEST_DYN_LINK
#ifdef STAND_ALONE
#   define BOOST_TEST_MODULE Main
#endif
#include <boost/test/unit_test.hpp>

#include "dom/JSONValue.h"

namespace JSONValueTest {

    void assertJSONValue(std::string expectedName, std::string expectedToString, const cadf::dom::JSONValue &actual) {
        BOOST_CHECK_EQUAL(expectedName, actual.getName());
        BOOST_CHECK_EQUAL(expectedToString, actual.toString());
        BOOST_CHECK_EQUAL("", actual.valueToString());
    }
}

/**
 * Test suite for base JSONValue class
 */
BOOST_AUTO_TEST_SUITE(JSONValue_Test_Suite)

/**
 * Verify that the created JSONValue contains the data as expected
 */
    BOOST_AUTO_TEST_CASE(CreateTest) {
        JSONValueTest::assertJSONValue("MyName", "\"MyName\":", cadf::dom::JSONValue("MyName"));
        JSONValueTest::assertJSONValue("JSON Value", "\"JSON Value\":", cadf::dom::JSONValue("JSON Value"));
        JSONValueTest::assertJSONValue("Some other value", "\"Some other value\":", cadf::dom::JSONValue("Some other value"));
    }

    BOOST_AUTO_TEST_SUITE_END()
