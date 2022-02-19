
#define BOOST_TEST_DYN_LINK
#ifdef STAND_ALONE
#   define BOOST_TEST_MODULE Main
#endif
#include <boost/test/unit_test.hpp>

#include "dom/JsonConverter.h"

BOOST_AUTO_TEST_SUITE(Json_Test_Suite)

    BOOST_AUTO_TEST_CASE(TreeWithMultipleValuesTest) {
        cadf::dom::DomNode root;
        root["Test Node"]["val1"] = 1;
        root["Test Node"]["val2"] = "Value 2";
        root["Test Node"]["val3"] = 5.40;
        root["Test Node"]["val4"] = -5;

        cadf::dom::DomConverter *converter = cadf::dom::json::JsonConverter::instance();

        // Verify the conversion to string
        BOOST_CHECK_EQUAL(62, converter->size(root));
        std::string jsonStr = converter->toString(root);
        std::string expected = "{\"Test Node\":{\"val1\":1,\"val2\":\"Value 2\",\"val3\":5.4,\"val4\":-5}}";
        BOOST_CHECK_EQUAL(expected, jsonStr);

        // Verify the conversion back
        cadf::dom::DomNode newRoot = converter->fromString(jsonStr);
        BOOST_CHECK_EQUAL(expected, converter->toString(newRoot));
        int val1 = newRoot["Test Node"]["val1"];
        BOOST_CHECK_EQUAL(1, val1);
        std::string val2 = newRoot["Test Node"]["val2"];
        BOOST_CHECK_EQUAL("Value 2", val2);
        double val3 = newRoot["Test Node"]["val3"];
        BOOST_CHECK_EQUAL(5.4, val3);
        int val4 = newRoot["Test Node"]["val4"];
        BOOST_CHECK_EQUAL(-5, val4);
    }

    BOOST_AUTO_TEST_CASE(SimpleTreeTest) {
        cadf::dom::DomNode root;
        root["int1"] = 123;
        root["int2"] = 234;
        root["int3"] = 345;
        root["String value"] = "This is a string!";
        root["Parent Node"]["int"] = 123;
        root["Parent Node"]["double"] = 1.23;
        root["Parent Node"]["long"] = 123123123;

        cadf::dom::DomConverter *converter = cadf::dom::json::JsonConverter::instance();

        // Verify the conversion to string
        BOOST_CHECK_EQUAL(126, converter->size(root));
        std::string jsonStr = converter->toString(root);
        std::string expected = "{\"Parent Node\":{\"double\":1.23,\"int\":123,\"long\":123123123},\"String value\":\"This is a string!\",\"int1\":123,\"int2\":234,\"int3\":345}";
        BOOST_CHECK_EQUAL(expected, jsonStr);

        // Verify the conversion back
        cadf::dom::DomNode newRoot = converter->fromString(jsonStr);
        BOOST_CHECK_EQUAL(expected, converter->toString(newRoot));
        BOOST_CHECK_EQUAL(123, root["int1"].operator int());
        BOOST_CHECK_EQUAL(234, root["int2"].operator int());
        BOOST_CHECK_EQUAL(345, root["int3"].operator int());
        BOOST_CHECK_EQUAL("This is a string!", root["String value"].operator std::string());
        BOOST_CHECK_EQUAL(123, root["Parent Node"]["int"].operator int());
        BOOST_CHECK_EQUAL(1.23, root["Parent Node"]["double"].operator double());
        BOOST_CHECK_EQUAL(123123123, root["Parent Node"]["long"].operator long());
    }

    BOOST_AUTO_TEST_CASE(MenuExampleTest) {
        cadf::dom::DomNode root;
        root["menu"]["id"] = "file";
        root["menu"]["value"] = "File";

        cadf::dom::DomNode menuItem1 = cadf::dom::buildNode("onclick", "CreateNewDoc()");
        menuItem1["value"] = "New";
        cadf::dom::DomNode menuItem2 = cadf::dom::buildNode("onclick", "OpenDoc()");
        menuItem2["value"] = "Open";
        cadf::dom::DomNode menuItem3 = cadf::dom::buildNode("onclick", "CloseDoc()");
        menuItem3["value"] = "Close";
        root["menu"]["popup"] = cadf::dom::buildNode("menuitem", {menuItem1, menuItem2, menuItem3});

        cadf::dom::DomConverter *converter = cadf::dom::json::JsonConverter::instance();

        // Verify the conversion to string
        BOOST_CHECK_EQUAL(183, converter->size(root));
        std::string jsonStr = converter->toString(root);
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
        BOOST_CHECK_EQUAL(expected, jsonStr);

        // Verify the conversion back
        cadf::dom::DomNode newRoot = converter->fromString(jsonStr);
        BOOST_CHECK_EQUAL(expected, converter->toString(newRoot));
        BOOST_CHECK_EQUAL("file", newRoot["menu"]["id"].operator std::string());
        BOOST_CHECK_EQUAL("File", newRoot["menu"]["value"].operator std::string());
        cadf::dom::DomNode menuSub = newRoot["menu"]["popup"]["menuitem"];
        BOOST_CHECK(menuSub.isArray());
        BOOST_CHECK_EQUAL(3, menuSub.numArrayElements());
        cadf::dom::DomNode::ArrayIterator it = menuSub.beginArray();
        BOOST_CHECK_EQUAL("CreateNewDoc()", it->operator[]("onclick").operator std::string());
        BOOST_CHECK_EQUAL("New", it->operator[]("value").operator std::string());
        it++;
        BOOST_CHECK_EQUAL("OpenDoc()", it->operator[]("onclick").operator std::string());
        BOOST_CHECK_EQUAL("Open", it->operator[]("value").operator std::string());
        it++;
        BOOST_CHECK_EQUAL("CloseDoc()", it->operator[]("onclick").operator std::string());
        BOOST_CHECK_EQUAL("Close", it->operator[]("value").operator std::string());
        it++;
        BOOST_CHECK(menuSub.endArray() == it);
    }

    BOOST_AUTO_TEST_CASE(CarMakeExampleTest) {
        cadf::dom::DomNode ford =cadf::dom::buildNode("models", {"Fiesta", "Focus", "Mustang"});
        ford["name"] = "Ford";
        cadf::dom::DomNode bmw =cadf::dom::buildNode("models", {"320", "X3", "X5"});
        bmw["name"] = "BMW";
        cadf::dom::DomNode fiat =cadf::dom::buildNode("models", {"500","Panda"});
        fiat["name"] = "Fiat";
        cadf::dom::DomNode root = cadf::dom::buildNode("cars", {ford, bmw, fiat});
        root["age"] = 30;
        root["name"] = "John";

        cadf::dom::DomConverter *converter = cadf::dom::json::JsonConverter::instance();

        // Verify the conversion to string
        BOOST_CHECK_EQUAL(170, converter->size(root));
        std::string jsonStr = converter->toString(root);
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
        BOOST_CHECK_EQUAL(expected, jsonStr);

        // Verify the conversion back
        cadf::dom::DomNode newRoot = converter->fromString(jsonStr);
        BOOST_CHECK_EQUAL(jsonStr, converter->toString(newRoot));
        BOOST_CHECK_EQUAL(30, newRoot["age"].operator int());
        BOOST_CHECK_EQUAL("John", newRoot["name"].operator std::string());
        cadf::dom::DomNode carSub = newRoot["cars"];
        BOOST_CHECK(carSub.isArray());
        BOOST_CHECK_EQUAL(3, carSub.numArrayElements());

        cadf::dom::DomNode::ArrayIterator it = carSub.beginArray();
        BOOST_CHECK_EQUAL("Ford", it->operator[]("name").operator std::string());
        BOOST_CHECK(it->operator[]("models").isArray());
        BOOST_CHECK_EQUAL(3, it->operator[]("models").numArrayElements());
        cadf::dom::DomNode::ArrayIterator subIt = it->operator[]("models").beginArray();
        BOOST_CHECK_EQUAL("Fiesta", subIt->operator std::string());
        subIt++;
        BOOST_CHECK_EQUAL("Focus", subIt->operator std::string());
        subIt++;
        BOOST_CHECK_EQUAL("Mustang", subIt->operator std::string());
        subIt++;
        BOOST_CHECK(subIt == it->operator[]("models").endArray());

        it++;
        BOOST_CHECK_EQUAL("BMW", it->operator[]("name").operator std::string());
        BOOST_CHECK(it->operator[]("models").isArray());
        BOOST_CHECK_EQUAL(3, it->operator[]("models").numArrayElements());
        subIt = it->operator[]("models").beginArray();
        BOOST_CHECK_EQUAL("320", subIt->operator std::string());
        subIt++;
        BOOST_CHECK_EQUAL("X3", subIt->operator std::string());
        subIt++;
        BOOST_CHECK_EQUAL("X5", subIt->operator std::string());
        subIt++;
        BOOST_CHECK(subIt == it->operator[]("models").endArray());

        it++;
        BOOST_CHECK_EQUAL("Fiat", it->operator[]("name").operator std::string());
        BOOST_CHECK(it->operator[]("models").isArray());
        BOOST_CHECK_EQUAL(2, it->operator[]("models").numArrayElements());
        subIt = it->operator[]("models").beginArray();
        BOOST_CHECK_EQUAL("500", subIt->operator std::string());
        subIt++;
        BOOST_CHECK_EQUAL("Panda", subIt->operator std::string());
        subIt++;
        BOOST_CHECK(subIt == it->operator[]("models").endArray());

        it++;
        BOOST_CHECK(it == carSub.endArray());
    }

    BOOST_AUTO_TEST_CASE(NullValuesTest) {
        cadf::dom::DomNode root = cadf::dom::buildNode("array", {cadf::dom::DomNode()});
        root["value"];

        cadf::dom::DomConverter *converter = cadf::dom::json::JsonConverter::instance();

        // Verify the conversion to string
        BOOST_CHECK_EQUAL(29, converter->size(root));
        std::string jsonStr = converter->toString(root);
        std::string expected = "{\"array\":[null],\"value\":null}";
        BOOST_CHECK_EQUAL(expected, jsonStr);

        // Verify the conversion back
        cadf::dom::DomNode newRoot = converter->fromString(jsonStr);
        BOOST_CHECK_EQUAL(expected, converter->toString(newRoot));
        BOOST_CHECK(newRoot["value"].isNull());
        cadf::dom::DomNode arrSub = newRoot["array"];
        BOOST_CHECK(arrSub.isArray());
        BOOST_CHECK_EQUAL(1, arrSub.numArrayElements());
        cadf::dom::DomNode::ArrayIterator it = arrSub.beginArray();
        BOOST_CHECK(it->isNull());
        it++;
        BOOST_CHECK(arrSub.endArray() == it);
    }

    BOOST_AUTO_TEST_SUITE_END()
