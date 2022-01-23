#define BOOST_TEST_DYN_LINK
#ifdef STAND_ALONE
#   define BOOST_TEST_MODULE Main
#endif
#include <boost/test/unit_test.hpp>
#include <fakeit.hpp>

#include "comms/network/serializer/json/JSONNode.h"

namespace JSONNodeTest {
    /*
     * Helper fixture for initializing and preparing all of the necessary mocks
     */
    struct SetupMocks {

            std::string VAL_1_NAME = "Value1";
            std::string VAL_2_NAME = "Value2";
            std::string VAL_3_NAME = "Value3";

            std::string VAL_1_TO_STRING = "VALUE 1";
            std::string VAL_2_TO_STRING = "VALUE 2";
            std::string VAL_3_TO_STRING = "VALUE 3";

            std::string VAL_1_VALUE = "VALUE 1 Value";
            std::string VAL_2_VALUE = "VALUE 2 Value";
            std::string VAL_3_VALUE = "VALUE 3 Value";

            SetupMocks() {
                fakeit::When(Method(mockValue1, getName)).AlwaysReturn(VAL_1_NAME);
                fakeit::When(Method(mockValue2, getName)).AlwaysReturn(VAL_2_NAME);
                fakeit::When(Method(mockValue3, getName)).AlwaysReturn(VAL_3_NAME);

                fakeit::When(Method(mockValue1, toString)).AlwaysReturn(VAL_1_TO_STRING);
                fakeit::When(Method(mockValue2, toString)).AlwaysReturn(VAL_2_TO_STRING);
                fakeit::When(Method(mockValue3, toString)).AlwaysReturn(VAL_3_TO_STRING);

                fakeit::When(Method(mockValue1, valueToString)).AlwaysReturn(VAL_1_VALUE);
                fakeit::When(Method(mockValue2, valueToString)).AlwaysReturn(VAL_2_VALUE);
                fakeit::When(Method(mockValue3, valueToString)).AlwaysReturn(VAL_3_VALUE);
            }

            ~SetupMocks() {
                verifyAllMocksChecked();
            }

            void verifyAllMocksChecked() {
                fakeit::VerifyNoOtherInvocations(mockValue1, mockValue2, mockValue3);
            }

            // Mocks to use for testing
            fakeit::Mock<cadf::comms::json::JSONValue> mockValue1;
            fakeit::Mock<cadf::comms::json::JSONValue> mockValue2;
            fakeit::Mock<cadf::comms::json::JSONValue> mockValue3;
    };

    /**
     * Helper fixture to initialize the LocalConnection instance that is to be used for the tests
     */
    struct TestFixture: public SetupMocks {

            TestFixture() : SetupMocks(), node("TestNode") {
                BOOST_CHECK_EQUAL("TestNode", node.getName());
                BOOST_CHECK_EQUAL("\"TestNode\":{}", node.toString());

                // Verify the initialization of the class to be tested
                verifyAllMocksChecked();
            }

            // The class to test
            cadf::comms::json::JSONNode node;
    };
}

/**
 * Test suite for validating the JSON Node
 */
BOOST_AUTO_TEST_SUITE(JSONNode_Test_Suite)

/**
 * Verify that can create a node with a single value
 */
    BOOST_FIXTURE_TEST_CASE(CreateNodeWithSingleValueTest, JSONNodeTest::TestFixture) {
        // Create the node
        node.setValue(&mockValue1.get());
        fakeit::Verify(Method(mockValue1, getName)).Exactly(1);

        // Verify that can retrieve the value
        const std::vector<cadf::comms::json::JSONValue*> values = node.getValue(VAL_1_NAME);
        BOOST_CHECK_EQUAL(1, values.size());
        BOOST_CHECK_EQUAL(&mockValue1.get(), values[0]);

        // Verify the generated string
        BOOST_CHECK_EQUAL("\"TestNode\":{" + VAL_1_TO_STRING + "}", node.toString());
        fakeit::Verify(Method(mockValue1, toString)).Exactly(1);
    }

    /**
     * Verify that can create a node with a multiple values
     */
    BOOST_FIXTURE_TEST_CASE(CreateNodeWithMultipleValuesTest, CreateNodeWithSingleValueTest) {
        CreateNodeWithSingleValueTest::test_method();

        // Add the second value
        node.setValue(&mockValue2.get());
        fakeit::Verify(Method(mockValue2, getName)).Exactly(1);

        const std::vector<cadf::comms::json::JSONValue*> &valuesVal2 = node.getValue(VAL_2_NAME);
        BOOST_CHECK_EQUAL(1, valuesVal2.size());
        BOOST_CHECK_EQUAL(&mockValue2.get(), valuesVal2[0]);

        BOOST_CHECK_EQUAL("\"TestNode\":{" + VAL_1_TO_STRING + "," + VAL_2_TO_STRING + "}", node.toString());
        fakeit::Verify(Method(mockValue1, toString)).Exactly(2);
        fakeit::Verify(Method(mockValue2, toString)).Exactly(1);

        // Add the third value
        node.setValue(&mockValue3.get());
        fakeit::Verify(Method(mockValue3, getName)).Exactly(1);

        const std::vector<cadf::comms::json::JSONValue*> &valuesVal3 = node.getValue(VAL_3_NAME);
        BOOST_CHECK_EQUAL(1, valuesVal3.size());
        BOOST_CHECK_EQUAL(&mockValue3.get(), valuesVal3[0]);

        BOOST_CHECK_EQUAL("\"TestNode\":{" + VAL_1_TO_STRING + "," + VAL_2_TO_STRING + "," + VAL_3_TO_STRING + "}", node.toString());
        fakeit::Verify(Method(mockValue1, toString)).Exactly(3);
        fakeit::Verify(Method(mockValue2, toString)).Exactly(2);
        fakeit::Verify(Method(mockValue3, toString)).Exactly(1);
    }

    /**
     * Verify that can create a node with an Array of values
     */
    BOOST_FIXTURE_TEST_CASE(CreateNodeWithArrayValuesTest, CreateNodeWithSingleValueTest) {
        CreateNodeWithSingleValueTest::test_method();

        fakeit::When(Method(mockValue2, getName)).AlwaysReturn(VAL_1_NAME);
        fakeit::When(Method(mockValue3, getName)).AlwaysReturn(VAL_1_NAME);

        // Add the second value
        node.setValue(&mockValue2.get());
        fakeit::Verify(Method(mockValue2, getName)).Exactly(1);

        const std::vector<cadf::comms::json::JSONValue*> &valuesVal2 = node.getValue(VAL_1_NAME);
        BOOST_CHECK_EQUAL(2, valuesVal2.size());
        BOOST_CHECK_EQUAL(&mockValue1.get(), valuesVal2[0]);
        BOOST_CHECK_EQUAL(&mockValue2.get(), valuesVal2[1]);

        BOOST_CHECK_EQUAL("\"TestNode\":{\"" + VAL_1_NAME + "\":[" + VAL_1_VALUE + "," + VAL_2_VALUE + "]}", node.toString());
        fakeit::Verify(Method(mockValue1, valueToString)).Exactly(1);
        fakeit::Verify(Method(mockValue2, valueToString)).Exactly(1);

        // Add the third value
        node.setValue(&mockValue3.get());
        fakeit::Verify(Method(mockValue3, getName)).Exactly(1);

        const std::vector<cadf::comms::json::JSONValue*> &valuesVal3 = node.getValue(VAL_1_NAME);
        BOOST_CHECK_EQUAL(3, valuesVal3.size());
        BOOST_CHECK_EQUAL(&mockValue1.get(), valuesVal3[0]);
        BOOST_CHECK_EQUAL(&mockValue2.get(), valuesVal3[1]);
        BOOST_CHECK_EQUAL(&mockValue3.get(), valuesVal3[2]);

        BOOST_CHECK_EQUAL("\"TestNode\":{\"" + VAL_1_NAME + "\":[" + VAL_1_VALUE + "," + VAL_2_VALUE + "," + VAL_3_VALUE + "]}", node.toString());
        fakeit::Verify(Method(mockValue1, valueToString)).Exactly(2);
        fakeit::Verify(Method(mockValue2, valueToString)).Exactly(2);
        fakeit::Verify(Method(mockValue3, valueToString)).Exactly(1);
    }

    /**
     * Verify that can create a node with an individual value and an Array of values
     */
    BOOST_FIXTURE_TEST_CASE(CreateNodeWithSingleValueAndArrayValuesTest, CreateNodeWithSingleValueTest) {
        CreateNodeWithSingleValueTest::test_method();

        fakeit::When(Method(mockValue2, getName)).AlwaysReturn(VAL_2_NAME);
        fakeit::When(Method(mockValue3, getName)).AlwaysReturn(VAL_2_NAME);

        // Add the second value
        node.setValue(&mockValue2.get());
        fakeit::Verify(Method(mockValue2, getName)).Exactly(1);

        std::vector<cadf::comms::json::JSONValue*> values = node.getValue(VAL_1_NAME);
        BOOST_CHECK_EQUAL(1, values.size());
        BOOST_CHECK_EQUAL(&mockValue1.get(), values[0]);

        values = node.getValue(VAL_2_NAME);
        BOOST_CHECK_EQUAL(1, values.size());
        BOOST_CHECK_EQUAL(&mockValue2.get(), values[0]);

        BOOST_CHECK_EQUAL("\"TestNode\":{" + VAL_1_TO_STRING + "," + VAL_2_TO_STRING + "}", node.toString());
        fakeit::Verify(Method(mockValue1, toString)).Exactly(2);
        fakeit::Verify(Method(mockValue2, toString)).Exactly(1);

        // Add the third value
        node.setValue(&mockValue3.get());
        fakeit::Verify(Method(mockValue3, getName)).Exactly(1);

        values = node.getValue(VAL_1_NAME);
        BOOST_CHECK_EQUAL(1, values.size());
        BOOST_CHECK_EQUAL(&mockValue1.get(), values[0]);

        values = node.getValue(VAL_2_NAME);
        BOOST_CHECK_EQUAL(2, values.size());
        BOOST_CHECK_EQUAL(&mockValue2.get(), values[0]);
        BOOST_CHECK_EQUAL(&mockValue3.get(), values[1]);

        BOOST_CHECK_EQUAL("\"TestNode\":{" + VAL_1_TO_STRING + ",\"" + VAL_2_NAME + "\":[" + VAL_2_VALUE + "," + VAL_3_VALUE + "]}", node.toString());
        fakeit::Verify(Method(mockValue1, toString)).Exactly(3);
        fakeit::Verify(Method(mockValue2, valueToString)).Exactly(1);
        fakeit::Verify(Method(mockValue3, valueToString)).Exactly(1);
    }

    /**
     * Verify that an exception is throw when trying to add an invalid value
     */
    BOOST_FIXTURE_TEST_CASE(SetInvalidValueTest, JSONNodeTest::TestFixture) {
        BOOST_REQUIRE_THROW(node.setValue(NULL), cadf::comms::json::JSONTreeException);

        fakeit::When(Method(mockValue1, getName)).AlwaysReturn("");
        BOOST_REQUIRE_THROW(node.setValue(&mockValue1.get()), cadf::comms::json::JSONTreeException);
        fakeit::Verify(Method(mockValue1, getName)).Exactly(1);
    }

    /**
     * Verify that an exception is throw when trying to add an invalid value
     */
    BOOST_FIXTURE_TEST_CASE(GetInvalidValueTest, JSONNodeTest::TestFixture) {
        BOOST_REQUIRE_THROW(node.getValue(""), cadf::comms::json::JSONTreeException);
        BOOST_REQUIRE_THROW(node.getValue(VAL_1_NAME), cadf::comms::json::JSONTreeException);
        BOOST_REQUIRE_THROW(node.getValue(VAL_2_NAME), cadf::comms::json::JSONTreeException);
        BOOST_REQUIRE_THROW(node.getValue(VAL_3_NAME), cadf::comms::json::JSONTreeException);
        BOOST_REQUIRE_THROW(node.getValue("THIS NAME DOES NOT EXIST"), cadf::comms::json::JSONTreeException);
    }

    BOOST_AUTO_TEST_SUITE_END()
