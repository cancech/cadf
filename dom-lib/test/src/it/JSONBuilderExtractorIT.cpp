#define BOOST_TEST_DYN_LINK
#ifdef STAND_ALONE
#   define BOOST_TEST_MODULE Main
#endif
#include <boost/test/unit_test.hpp>
#include <iostream>

#include "dom/JSONBuilder.h"
#include "dom/JSONExtractor.h"

namespace JSONBuilderExtractorTest {

    /**
     * Dummy sub-class of the JSONValue for use in testing
     */
    class DummyValue : public camb::dom::JSONValue {
        public:
            DummyValue() : JSONValue("DummyValue") {
            }
    };
}

/**
 * Test suite for the construction and validation of JSON Trees built using the builder and extracted with the extractor
 */
BOOST_AUTO_TEST_SUITE(JSONBuilderExtractor_Test_Suite)

/**
 * Verify that can create a node that contains multiple values
 */
    BOOST_AUTO_TEST_CASE(CreateNodeWithMultipleValuesTest) {
        camb::dom::JSONBuilder builder;
        camb::dom::JSONValue *node = builder.createNode("Test Node");
        builder.addValue(node, "val1", 1);
        builder.addValue<std::string>(node, "val2", "Value 2");
        builder.addValue(node, "val3", 5.40);
        builder.addValue(node, "val4", -5);
        const camb::dom::JSONValue *root = builder.getRoot();

        BOOST_CHECK_EQUAL("{\"Test Node\":{\"val1\":1,\"val2\":\"Value 2\",\"val3\":5.4,\"val4\":-5}}", root->toString());

        camb::dom::JSONExtractor extractor(root);
        const camb::dom::JSONValue *testNode = extractor.getChild("Test Node");
        BOOST_CHECK_EQUAL("Test Node", testNode->getName());
        BOOST_CHECK_EQUAL(node, testNode);

        BOOST_CHECK_EQUAL(1, extractor.getValue<int>(testNode, "val1"));
        BOOST_CHECK_EQUAL("Value 2", extractor.getValue<std::string>(testNode, "val2"));
        BOOST_CHECK_EQUAL(5.40, extractor.getValue<double>(testNode, "val3"));
        BOOST_CHECK_EQUAL(-5, extractor.getValue<int>(testNode, "val4"));

        // Try retrieving a value as a child
        const camb::dom::JSONValue *val1Node = extractor.getChild(testNode, "val1");
        BOOST_CHECK_EQUAL(1, extractor.getValue<int>(val1Node, "val1"));

        delete (root);
    }

    /**
     * Verify that can create a simple tree
     */
    BOOST_AUTO_TEST_CASE(CreateSimpleTreeTest) {
        // Build the tree
        camb::dom::JSONBuilder builder;
        builder.addValue("int1", 123);
        builder.addValue("int2", 234);
        builder.addValue("int3", 345);
        builder.addValue<std::string>("String value", "This is a string!");

        camb::dom::JSONValue *parentNode = builder.createNode("Parent Node");
        builder.addValue(parentNode, "int", 123);
        builder.addValue(parentNode, "double", 1.23);
        builder.addValue(parentNode, "long", 123123123);

        const camb::dom::JSONValue *root = builder.getRoot();
        BOOST_CHECK_EQUAL("{\"Parent Node\":{\"double\":1.23,\"int\":123,\"long\":123123123},\"String value\":\"This is a string!\",\"int1\":123,\"int2\":234,\"int3\":345}", root->toString());

        // Verify the values in the tree
        camb::dom::JSONExtractor extractor(root);
        BOOST_CHECK_EQUAL(123, extractor.getValue<int>("int1"));
        BOOST_CHECK_EQUAL(234, extractor.getValue<int>("int2"));
        BOOST_CHECK_EQUAL(345, extractor.getValue<int>("int3"));
        BOOST_CHECK_EQUAL("This is a string!", extractor.getValue<std::string>("String value"));

        const camb::dom::JSONValue *subNode = extractor.getChild("Parent Node");
        BOOST_CHECK_EQUAL(parentNode, subNode);
        BOOST_CHECK_EQUAL(123, extractor.getValue<int>(subNode, "int"));
        BOOST_CHECK_EQUAL(1.23, extractor.getValue<double>(subNode, "double"));
        BOOST_CHECK_EQUAL(123123123, extractor.getValue<long>(subNode, "long"));

        delete (root);
    }

    /**
     * Verify that can create a node representing an array of values
     */
    BOOST_AUTO_TEST_CASE(MenuExampleTest) {
        camb::dom::JSONBuilder builder;

        camb::dom::JSONValue *menu = builder.createNode("menu");
        builder.addValue<std::string>(menu, "id", "file");
        builder.addValue<std::string>(menu, "value", "File");

        camb::dom::JSONValue *popup = builder.createNode(menu, "popup");

        camb::dom::JSONValue *menuItem1 = builder.createNode(popup, "menuitem");
        builder.addValue<std::string>(menuItem1, "value", "New");
        builder.addValue<std::string>(menuItem1, "onclick", "CreateNewDoc()");

        camb::dom::JSONValue *menuItem2 = builder.createNode(popup, "menuitem");
        builder.addValue<std::string>(menuItem2, "value", "Open");
        builder.addValue<std::string>(menuItem2, "onclick", "OpenDoc()");

        camb::dom::JSONValue *menuItem3 = builder.createNode(popup, "menuitem");
        builder.addValue<std::string>(menuItem3, "value", "Close");
        builder.addValue<std::string>(menuItem3, "onclick", "CloseDoc()");

        const camb::dom::JSONValue *root = builder.getRoot();

        std::string expected = "{\"menu\":{"
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
        BOOST_CHECK_EQUAL(expected, root->toString());

        // Verify the contents of the tree
        camb::dom::JSONExtractor extractor(root);
        const camb::dom::JSONValue *subNodeMenu = extractor.getChild("menu");
        BOOST_CHECK_EQUAL("file", extractor.getValue<std::string>(subNodeMenu, "id"));
        BOOST_CHECK_EQUAL("File", extractor.getValue<std::string>(subNodeMenu, "value"));

        const camb::dom::JSONValue *subNodePopup = extractor.getChild(subNodeMenu, "popup");
        const std::vector<camb::dom::JSONValue*> &subNodesMenuItem = extractor.getChildArray(subNodePopup, "menuitem");
        BOOST_CHECK_EQUAL(3, subNodesMenuItem.size());

        BOOST_CHECK_EQUAL("New", extractor.getValue<std::string>(subNodesMenuItem[0], "value"));
        BOOST_CHECK_EQUAL("CreateNewDoc()", extractor.getValue<std::string>(subNodesMenuItem[0], "onclick"));
        BOOST_CHECK_EQUAL("Open", extractor.getValue<std::string>(subNodesMenuItem[1], "value"));
        BOOST_CHECK_EQUAL("OpenDoc()", extractor.getValue<std::string>(subNodesMenuItem[1], "onclick"));
        BOOST_CHECK_EQUAL("Close", extractor.getValue<std::string>(subNodesMenuItem[2], "value"));
        BOOST_CHECK_EQUAL("CloseDoc()", extractor.getValue<std::string>(subNodesMenuItem[2], "onclick"));

        delete (root);
    }

    /**
     * Verify that can create a node representing an array of values
     */
    BOOST_AUTO_TEST_CASE(CarMakeExampleTest) {
        camb::dom::JSONBuilder builder;
        builder.addValue<std::string>("name", "John");
        builder.addValue("age", 30);

        camb::dom::JSONValue *car1 = builder.createNode("cars");
        builder.addValue<std::string>(car1, "name", "Ford");
        builder.addValue<std::string>(car1, "models", "Fiesta");
        builder.addValue<std::string>(car1, "models", "Focus");
        builder.addValue<std::string>(car1, "models", "Mustang");

        camb::dom::JSONValue *car2 = builder.createNode("cars");
        builder.addValue<std::string>(car2, "name", "BMW");
        builder.addValue<std::string>(car2, "models", "320");
        builder.addValue<std::string>(car2, "models", "X3");
        builder.addValue<std::string>(car2, "models", "X5");

        camb::dom::JSONValue *car3 = builder.createNode("cars");
        builder.addValue<std::string>(car3, "name", "Fiat");
        builder.addValue<std::string>(car3, "models", "500");
        builder.addValue<std::string>(car3, "models", "Panda");

        const camb::dom::JSONValue *root = builder.getRoot();

        std::string expected = "{"
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
        BOOST_CHECK_EQUAL(expected, root->toString());

        // Verify contents of the tree
        camb::dom::JSONExtractor extractor(root);
        BOOST_CHECK_EQUAL(30, extractor.getValue<int>("age"));
        BOOST_CHECK_EQUAL("John", extractor.getValue<std::string>("name"));

        const std::vector<camb::dom::JSONValue*> &subNodeCars = extractor.getChildArray("cars");
        BOOST_CHECK_EQUAL(3, subNodeCars.size());

        BOOST_CHECK_EQUAL("Ford", extractor.getValue<std::string>(subNodeCars[0], "name"));
        std::vector<std::string> subNodeModel = extractor.getValueArray<std::string>(subNodeCars[0], "models");
        BOOST_CHECK_EQUAL(3, subNodeModel.size());
        BOOST_CHECK_EQUAL("Fiesta", subNodeModel[0]);
        BOOST_CHECK_EQUAL("Focus", subNodeModel[1]);
        BOOST_CHECK_EQUAL("Mustang", subNodeModel[2]);

        BOOST_CHECK_EQUAL("BMW", extractor.getValue<std::string>(subNodeCars[1], "name"));
        subNodeModel = extractor.getValueArray<std::string>(subNodeCars[1], "models");
        BOOST_CHECK_EQUAL(3, subNodeModel.size());
        BOOST_CHECK_EQUAL("320", subNodeModel[0]);
        BOOST_CHECK_EQUAL("X3", subNodeModel[1]);
        BOOST_CHECK_EQUAL("X5", subNodeModel[2]);

        BOOST_CHECK_EQUAL("Fiat", extractor.getValue<std::string>(subNodeCars[2], "name"));
        subNodeModel = extractor.getValueArray<std::string>(subNodeCars[2], "models");
        BOOST_CHECK_EQUAL(2, subNodeModel.size());
        BOOST_CHECK_EQUAL("500", subNodeModel[0]);
        BOOST_CHECK_EQUAL("Panda", subNodeModel[1]);

        delete (root);
    }

    /**
     * Verify that the expected exceptions are thrown
     */
    BOOST_AUTO_TEST_CASE(ExceptionsThrownTest) {
        camb::dom::JSONBuilder builder;
        builder.addValue("ArrayVal", 123);
        builder.addValue("ArrayVal", 123);
        builder.createNode("ChildNode");

        const camb::dom::JSONValue *root = builder.getRoot();
        camb::dom::JSONPODValue podVal("pod", 123);

        camb::dom::JSONExtractor extractor(root);

        BOOST_REQUIRE_THROW(extractor.getChild("Doesn't Exist"), camb::dom::JSONTreeException);
        BOOST_REQUIRE_THROW(extractor.getChild("ArrayVal"), camb::dom::JSONTreeException);
        BOOST_REQUIRE_THROW(extractor.getChild(NULL, "ArrayVal"), camb::dom::JSONTreeException);
        BOOST_REQUIRE_THROW(extractor.getChild(&podVal, "ArrayVal"), camb::dom::JSONTreeException);

        BOOST_REQUIRE_THROW(extractor.getValue<std::string>("Doesn't Exist"), camb::dom::JSONTreeException);
        BOOST_REQUIRE_THROW(extractor.getValue<int>("ArrayVal"), camb::dom::JSONTreeException);
        BOOST_REQUIRE_THROW(extractor.getValue<std::string>("ChildNode"), camb::dom::JSONTreeException);
        BOOST_REQUIRE_THROW(extractor.getValue<int>(NULL, "ArrayVal"), camb::dom::JSONTreeException);
        BOOST_REQUIRE_THROW(extractor.getValue<int>(&podVal, "ArrayVal"), camb::dom::JSONTreeException);

        JSONBuilderExtractorTest::DummyValue dummyValue;
        BOOST_REQUIRE_THROW(extractor.getValue<int>(&dummyValue, "ArrayVal"), camb::dom::JSONTreeException);

        delete(root);
    }

    BOOST_AUTO_TEST_SUITE_END()