
#define BOOST_TEST_DYN_LINK
#ifdef STAND_ALONE
#   define BOOST_TEST_MODULE Main
#endif
#include <boost/test/unit_test.hpp>
#include <iostream>


#include "dom/JSONValue.h"
#include "dom/JSONPODValue.h"
#include "dom/JSONNode.h"
#include "dom/JSONRootNode.h"

namespace JSONTreeTest {
    template<typename T>
    void createAndAssertNode(std::string name, std::string key, const T &value, const std::string &expectedStr) {
        camb::dom::JSONPODValue podVal(key, value);
        BOOST_CHECK_EQUAL(key, podVal.getName());
        camb::dom::JSONNode node(name);
        BOOST_CHECK_EQUAL(name, node.getName());
        node.setValue(&podVal);
        camb::dom::JSONValue *actual = node.getValue(key)[0];
        BOOST_CHECK_EQUAL(&podVal, actual);
        BOOST_CHECK_EQUAL(expectedStr, node.toString());
    }

    template<typename T>
    T getValueFromNode(const camb::dom::JSONNode* node, const std::string &key) {
        const camb::dom::JSONValue *val = node->getValue(key)[0];
        return static_cast<const camb::dom::JSONPODValue*>(val)->getValue<T>();
    }

    const camb::dom::JSONNode* getNestedNodeFromNode(const camb::dom::JSONNode* node, const std::string &key) {
        const camb::dom::JSONValue *val = node->getValue(key)[0];
        return static_cast<const camb::dom::JSONNode*>(val);
    }
}

/**
 * Test suite for the construction and validation of JSON Trees
 */
BOOST_AUTO_TEST_SUITE(JSONTree_Test_Suite)

/**
 * Verify that can create nodes with simple POD types
 */
    BOOST_AUTO_TEST_CASE(CreateSingleValuePODNodeTest) {
        JSONTreeTest::createAndAssertNode<std::string>("String Node", "key", "String Value", "\"String Node\":{\"key\":\"String Value\"}");
        JSONTreeTest::createAndAssertNode<int>("Integer Node", "key", 13579, "\"Integer Node\":{\"key\":13579}");
        JSONTreeTest::createAndAssertNode<short>("Short Node", "key", 321, "\"Short Node\":{\"key\":321}");
        JSONTreeTest::createAndAssertNode<double>("Double Node", "key", 987.321, "\"Double Node\":{\"key\":987.321}");
        JSONTreeTest::createAndAssertNode<long>("Long Node", "key", 9876543210, "\"Long Node\":{\"key\":9876543210}");
        JSONTreeTest::createAndAssertNode<unsigned int>("Unsigned Integer Node", "key", 24680, "\"Unsigned Integer Node\":{\"key\":24680}");
        JSONTreeTest::createAndAssertNode<char>("Char Node", "key", 'r', "\"Char Node\":{\"key\":114}");
    }

    /**
     * Verify that can create a node that contains multiple values
     */
    BOOST_AUTO_TEST_CASE(CreateNodeWithMultipleValuesTest) {
        camb::dom::JSONNode node("Test Node");
        camb::dom::JSONPODValue val1("val1", 1);
        std::string val2Val = "Value 2";
        camb::dom::JSONPODValue val2("val2", val2Val);
        camb::dom::JSONPODValue val3("val3", 5.40);
        camb::dom::JSONPODValue val4("val4", -5);
        node.setValue(&val1);
        node.setValue(&val2);
        node.setValue(&val3);
        node.setValue(&val4);

        BOOST_CHECK_EQUAL(1, JSONTreeTest::getValueFromNode<int>(&node, "val1"));
        BOOST_CHECK_EQUAL("Value 2", JSONTreeTest::getValueFromNode<std::string>(&node, "val2"));
        BOOST_CHECK_EQUAL(5.40, JSONTreeTest::getValueFromNode<double>(&node, "val3"));
        BOOST_CHECK_EQUAL(-5, JSONTreeTest::getValueFromNode<short>(&node, "val4"));
        BOOST_CHECK_EQUAL("\"Test Node\":{\"val1\":1,\"val2\":\"Value 2\",\"val3\":5.4,\"val4\":-5}", node.toString());
    }

    /**
     * Verify that can create a simple tree
     */
    BOOST_AUTO_TEST_CASE(CreateSimpleTreeTest) {
        // Build the tree
        camb::dom::JSONNode root("root");

        camb::dom::JSONPODValue intVal1("int1", 123);
        camb::dom::JSONPODValue intVal2("int2", 234);
        camb::dom::JSONPODValue intVal3("int3", 345);
        root.setValue(&intVal1);
        root.setValue(&intVal2);
        root.setValue(&intVal3);

        std::string val = "This is a string!";
        camb::dom::JSONPODValue stringVal("String value", val);
        root.setValue(&stringVal);

        camb::dom::JSONNode parentNode("Parent Node");
        camb::dom::JSONPODValue parentIntVal("int", 123);
        camb::dom::JSONPODValue parentDblVal("double", 1.23);
        camb::dom::JSONPODValue parentLongVal("long", 123123123);
        parentNode.setValue(&parentIntVal);
        parentNode.setValue(&parentDblVal);
        parentNode.setValue(&parentLongVal);
        root.setValue(&parentNode);

        // Verify the values in the tree
        BOOST_CHECK_EQUAL(123, JSONTreeTest::getValueFromNode<int>(&root, "int1"));
        BOOST_CHECK_EQUAL(234, JSONTreeTest::getValueFromNode<int>(&root, "int2"));
        BOOST_CHECK_EQUAL(345, JSONTreeTest::getValueFromNode<int>(&root, "int3"));
        BOOST_CHECK_EQUAL("This is a string!", JSONTreeTest::getValueFromNode<std::string>(&root, "String value"));

        const camb::dom::JSONNode *subNode = JSONTreeTest::getNestedNodeFromNode(&root, "Parent Node");
        BOOST_CHECK_EQUAL(123, JSONTreeTest::getValueFromNode<int>(subNode, "int"));
        BOOST_CHECK_EQUAL(1.23, JSONTreeTest::getValueFromNode<double>(subNode, "double"));
        BOOST_CHECK_EQUAL(123123123, JSONTreeTest::getValueFromNode<long>(subNode, "long"));

        BOOST_CHECK_EQUAL("\"root\":{\"Parent Node\":{\"double\":1.23,\"int\":123,\"long\":123123123},\"String value\":\"This is a string!\",\"int1\":123,\"int2\":234,\"int3\":345}", root.toString());
    }

    /**
     * Verify that can create a node representing an array of values
     */
    BOOST_AUTO_TEST_CASE(MenuExampleTest) {
        camb::dom::JSONRootNode root;
        camb::dom::JSONNode menu("menu");
        root.setValue(&menu);

        std::string idVal = "file";
        camb::dom::JSONPODValue id("id", idVal);
        std::string valueVal = "File";
        camb::dom::JSONPODValue value("value", valueVal);
        menu.setValue(&id);
        menu.setValue(&value);

        camb::dom::JSONNode menuItem1("menuitem");
        std::string newStr = "New";
        camb::dom::JSONPODValue menuItem1Val("value", newStr);
        std::string newClickStr = "CreateNewDoc()";
        camb::dom::JSONPODValue menuItem1Click("onclick", newClickStr);
        menuItem1.setValue(&menuItem1Val);
        menuItem1.setValue(&menuItem1Click);

        camb::dom::JSONNode menuItem2("menuitem");
        std::string openStr = "Open";
        camb::dom::JSONPODValue menuItem2Val("value", openStr);
        std::string openClickStr = "OpenDoc()";
        camb::dom::JSONPODValue menuItem2Click("onclick", openClickStr);
        menuItem2.setValue(&menuItem2Val);
        menuItem2.setValue(&menuItem2Click);

        camb::dom::JSONNode menuItem3("menuitem");
        std::string closeStr = "Close";
        camb::dom::JSONPODValue menuItem3Val("value", closeStr);
        std::string closeClickStr = "CloseDoc()";
        camb::dom::JSONPODValue menuItem3Click("onclick", closeClickStr);
        menuItem3.setValue(&menuItem3Val);
        menuItem3.setValue(&menuItem3Click);


        camb::dom::JSONNode popup("popup");
        popup.setValue(&menuItem1);
        popup.setValue(&menuItem2);
        popup.setValue(&menuItem3);

        menu.setValue(&popup);

        std::string expected =
                "{\"menu\":{"
                    "\"id\":\"file\","
                    "\"popup\":{"
                        "\"menuitem\":["
                            "{\"onclick\":\"CreateNewDoc()\",\"value\":\"New\"},"
                            "{\"onclick\":\"OpenDoc()\",\"value\":\"Open\"},"
                            "{\"onclick\":\"CloseDoc()\",\"value\":\"Close\"}"
                        "]"
                    "},"
                    "\"value\":\"File\""
                "}}";
        BOOST_CHECK_EQUAL(expected, root.toString());
    }

    /**
     * Verify that can create a node representing an array of values
     */
    BOOST_AUTO_TEST_CASE(CarMakeExampleTest) {
        camb::dom::JSONRootNode root;
        std::string nameVal = "John";
        camb::dom::JSONPODValue name("name", nameVal);
        camb::dom::JSONPODValue age("age", 30);
        root.setValue(&name);
        root.setValue(&age);

        camb::dom::JSONNode car1("cars");
        std::string car1NameStr = "Ford";
        camb::dom::JSONPODValue car1Name("name", car1NameStr);
        car1.setValue(&car1Name);

        std::string car1Model1Str = "Fiesta";
        camb::dom::JSONPODValue car1Model1("models", car1Model1Str);
        std::string car1Model2Str = "Focus";
        camb::dom::JSONPODValue car1Model2("models", car1Model2Str);
        std::string car1Model3Str = "Mustang";
        camb::dom::JSONPODValue car1Model3("models", car1Model3Str);
        car1.setValue(&car1Model1);
        car1.setValue(&car1Model2);
        car1.setValue(&car1Model3);
        root.setValue(&car1);



        camb::dom::JSONNode car2("cars");
        std::string car2NameStr = "BMW";
        camb::dom::JSONPODValue car2Name("name", car2NameStr);
        car2.setValue(&car2Name);

        std::string car2Model1Str = "320";
        camb::dom::JSONPODValue car2Model1("models", car2Model1Str);
        std::string car2Model2Str = "X3";
        camb::dom::JSONPODValue car2Model2("models", car2Model2Str);
        std::string car2Model3Str = "X5";
        camb::dom::JSONPODValue car2Model3("models", car2Model3Str);
        car2.setValue(&car2Model1);
        car2.setValue(&car2Model2);
        car2.setValue(&car2Model3);
        root.setValue(&car2);



        camb::dom::JSONNode car3("cars");
        std::string car3NameStr = "Fiat";
        camb::dom::JSONPODValue car3Name("name", car3NameStr);
        car3.setValue(&car3Name);

        std::string car3Model1Str = "500";
        camb::dom::JSONPODValue car3Model1("models", car3Model1Str);
        std::string car3Model2Str = "Panda";
        camb::dom::JSONPODValue car3Model2("models", car3Model2Str);
        car3.setValue(&car3Model1);
        car3.setValue(&car3Model2);
        root.setValue(&car3);

        std::string expected =
                "{"
                    "\"age\":30,"
                    "\"cars\":[{"
                        "\"models\":[\"Fiesta\",\"Focus\",\"Mustang\"],"
                        "\"name\":\"Ford\""
                    "},{"
                        "\"models\":[\"320\",\"X3\",\"X5\"],"
                        "\"name\":\"BMW\""
                    "},{"
                        "\"models\":[\"500\",\"Panda\"],"
                        "\"name\":\"Fiat\""
                    "}],"
                    "\"name\":\"John\""
                "}";
        BOOST_CHECK_EQUAL(expected, root.toString());
    }

    BOOST_AUTO_TEST_SUITE_END()
