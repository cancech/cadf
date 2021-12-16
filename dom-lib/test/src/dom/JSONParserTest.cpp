#define BOOST_TEST_DYN_LINK
#ifdef STAND_ALONE
#   define BOOST_TEST_MODULE Main
#endif
#include <boost/test/unit_test.hpp>
#include <iostream>

#include "dom/JSONParser.h"
#include "dom/JSONExtractor.h"
#include "dom/JSONException.h"

/**
 * Test case for the JSONParser
 */
BOOST_AUTO_TEST_SUITE(JSONParser_Test_Suite)

/**
 * Verify that can create a load a tree that contains multiple values
 */
    BOOST_AUTO_TEST_CASE(CreateNodeWithMultipleValuesTest) {
        const camb::dom::JSONValue *root = camb::dom::JSONParser::parse("{\"Test Node\":{\"val1\":1,\"val2\":\"Value 2\",\"val3\":5.4,\"val4\":-5}}");

        camb::dom::JSONExtractor extractor(root);
        const camb::dom::JSONValue *testNode = extractor.getChild("Test Node");
        BOOST_CHECK_EQUAL("Test Node", testNode->getName());

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
     * Verify that can load a simple tree
     */
    BOOST_AUTO_TEST_CASE(CreateSimpleTreeTest) {
        const camb::dom::JSONValue *root = camb::dom::JSONParser::parse(
                "{\"Parent Node\":{\"double\":1.23,\"int\":123,\"long\":123123123},\"String value\":\"This is a string!\",\"int1\":123,\"int2\":234,\"int3\":345}");

        // Verify the values in the tree
        camb::dom::JSONExtractor extractor(root);
        BOOST_CHECK_EQUAL(123, extractor.getValue<int>("int1"));
        BOOST_CHECK_EQUAL(234, extractor.getValue<int>("int2"));
        BOOST_CHECK_EQUAL(345, extractor.getValue<int>("int3"));
        BOOST_CHECK_EQUAL("This is a string!", extractor.getValue<std::string>("String value"));

        const camb::dom::JSONValue *subNode = extractor.getChild("Parent Node");
        BOOST_CHECK_EQUAL(123, extractor.getValue<int>(subNode, "int"));
        BOOST_CHECK_EQUAL(1.23, extractor.getValue<double>(subNode, "double"));
        BOOST_CHECK_EQUAL(123123123, extractor.getValue<long>(subNode, "long"));

        delete (root);
    }

    /**
     * Verify that can load an array of values
     */
    BOOST_AUTO_TEST_CASE(MenuExampleTest) {
        std::string json = "{\"menu\":{"
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
        const camb::dom::JSONValue *root = camb::dom::JSONParser::parse(json);

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
     * Verify that can load nested arrays
     */
    BOOST_AUTO_TEST_CASE(CarMakeExampleTest) {
        std::string json = "{"
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
        const camb::dom::JSONValue *root = camb::dom::JSONParser::parse(json);

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
     * Verify that whitespace characters are properly skipped during parsing
     */
    BOOST_AUTO_TEST_CASE(WhitespaceExampleTest) {
        std::string json = "\t \n\v\t \n\v\t \n\v{\t \n\v\t \n\v\t \n\v\t \n\v"
                "\"age\"\t \n\v:\t \n\v30\t \n\v,\t \n\v"
                "\"cars\"\t \n\v:\t \n\v[\t \n\v{\t \n\v"
                "\"models\"\t \n\v:\t \n\v[\t \n\v\"Fiesta\"\t \n\v,\t \n\v\"Focus\"\t \n\v,\t \n\v\"Mustang\"\t \n\v]\t \n\v,\t \n\v"
                "\"name\"\t \n\v:\t \n\v\"Ford\"\t \n\v"
                "}\t \n\v,\t \n\v{\t \n\v"
                "\"models\"\t \n\v:\t \n\v[\t \n\v\"320\"\t \n\v,\t \n\v\"X3\"\t \n\v,\t \n\v\"X5\"\t \n\v]\t \n\v,\t \n\v"
                "\"name\"\t \n\v:\t \n\v\"BMW\"\t \n\v"
                "}\t \n\v,\t \n\v{\t \n\v"
                "\"models\"\t \n\v:\t \n\v[\t \n\v\"500\"\t \n\v,\t \n\v\"Panda\"\t \n\v]\t \n\v,\t \n\v"
                "\"name\"\t \n\v:\t \n\v\"Fiat\"\t \n\v"
                "}\t \n\v]\t \n\v,\t \n\v"
                "\"name\"\t \n\v:\t \n\v\"John\"\t \n\v"
                "}\t \n\v\t \n\v\t \n\v";
        const camb::dom::JSONValue *root = camb::dom::JSONParser::parse(json);

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
     * Verify that exceptions are properly thrown when the JSON is malformed.
     */
    BOOST_AUTO_TEST_CASE(MalformedJsonTest) {
        BOOST_REQUIRE_THROW(camb::dom::JSONParser::parse(""), std::out_of_range);
        BOOST_REQUIRE_THROW(camb::dom::JSONParser::parse("\"key\":123"), camb::dom::JSONParseException);
        BOOST_REQUIRE_THROW(camb::dom::JSONParser::parse("{\"key\":123,"), std::out_of_range);
        BOOST_REQUIRE_THROW(camb::dom::JSONParser::parse("{\"key\":123,"), std::out_of_range);
        BOOST_REQUIRE_THROW(camb::dom::JSONParser::parse("{\"key\":12 3}"), camb::dom::JSONParseException);
        BOOST_REQUIRE_THROW(camb::dom::JSONParser::parse("{\"key\":123]"), camb::dom::JSONParseException);
        BOOST_REQUIRE_THROW(camb::dom::JSONParser::parse("{key:123}"), camb::dom::JSONParseException);
        BOOST_REQUIRE_THROW(camb::dom::JSONParser::parse("{\"key\":[123}}"), camb::dom::JSONParseException);
        BOOST_REQUIRE_THROW(camb::dom::JSONParser::parse("{\"key\":}"), camb::dom::JSONParseException);
        BOOST_REQUIRE_THROW(camb::dom::JSONParser::parse("{\"key\":]}"), camb::dom::JSONParseException);
        BOOST_REQUIRE_THROW(camb::dom::JSONParser::parse("{\"key\":,}"), camb::dom::JSONParseException);
        BOOST_REQUIRE_THROW(camb::dom::JSONParser::parse("{\"key\"::}"), camb::dom::JSONParseException);
    }

    BOOST_AUTO_TEST_SUITE_END()
