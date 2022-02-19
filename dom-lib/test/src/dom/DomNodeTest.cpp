#define BOOST_TEST_DYN_LINK
#ifdef STAND_ALONE
#   define BOOST_TEST_MODULE Main
#endif
#include <boost/test/unit_test.hpp>
#include <limits>

#include "dom/DomNode.h"

namespace DomNodeTest {
    void verifyNodeState(const cadf::dom::DomNode &node, std::string stringVal, int intVal, double doubleVal, bool boolVal, char charVal,
            size_t numChildren, size_t numArray, bool isLeaf, bool isString, bool isArray, bool isNull) {
        BOOST_CHECK_EQUAL(stringVal, node.operator std::string());
        BOOST_CHECK_EQUAL(intVal, node.operator int());
        BOOST_CHECK_EQUAL(intVal, node.operator long());
        BOOST_CHECK_EQUAL(doubleVal, node.operator double());
        BOOST_CHECK_EQUAL(boolVal, node.operator bool());
        BOOST_CHECK_EQUAL(charVal, node.operator char());
        BOOST_CHECK_EQUAL(numChildren, node.numChildren());
        BOOST_CHECK_EQUAL(numArray, node.numArrayElements());
        BOOST_CHECK_EQUAL(isLeaf, node.isLeaf());
        BOOST_CHECK_EQUAL(isString, node.isString());
        BOOST_CHECK_EQUAL(isArray, node.isArray());
        BOOST_CHECK_EQUAL(isNull, node.isNull());
    }
}

BOOST_AUTO_TEST_SUITE(DomNode_Test_Suite)

    BOOST_AUTO_TEST_CASE(NullNodeTest) {
        cadf::dom::DomNode node;
        DomNodeTest::verifyNodeState(node, "", 0, 0, false, '\0', 0, 0, true, false, false, true);
    }

    BOOST_AUTO_TEST_CASE(CStringNodeTest) {
        cadf::dom::DomNode ctorNode("ABC 123");
        DomNodeTest::verifyNodeState(ctorNode, "ABC 123", 0, 0, false, 'A', 0, 0, true, true, false, false);

        cadf::dom::DomNode assignNode = "A VALUE";
        DomNodeTest::verifyNodeState(assignNode, "A VALUE", 0, 0, false, 'A', 0, 0, true, true, false, false);
    }

    BOOST_AUTO_TEST_CASE(StdStringNodeTest) {
        cadf::dom::DomNode ctorNode(std::string("ABC 123"));
        DomNodeTest::verifyNodeState(ctorNode, "ABC 123", 0, 0, false, 'A', 0, 0, true, true, false, false);

        cadf::dom::DomNode assignNode = std::string("A VALUE");
        DomNodeTest::verifyNodeState(assignNode, "A VALUE", 0, 0, false, 'A', 0, 0, true, true, false, false);
    }

    BOOST_AUTO_TEST_CASE(EmptyStringNodeTest) {
        cadf::dom::DomNode ctorNode("");
        DomNodeTest::verifyNodeState(ctorNode, "", 0, 0, false, '\0', 0, 0, true, true, false, false);

        cadf::dom::DomNode assignNode = "";
        DomNodeTest::verifyNodeState(assignNode, "", 0, 0, false, '\0', 0, 0, true, true, false, false);
    }

    BOOST_AUTO_TEST_CASE(IntNodeTest) {
        cadf::dom::DomNode ctorNode(4837);
        DomNodeTest::verifyNodeState(ctorNode, "4837", 4837, 4837, false, '4', 0, 0, true, false, false, false);

        cadf::dom::DomNode assignNode = 583;
        DomNodeTest::verifyNodeState(assignNode, "583", 583, 583, false, '5', 0, 0, true, false, false, false);
    }

    BOOST_AUTO_TEST_CASE(LongNodeTest) {
        long val = (long) std::numeric_limits<int>::max() + 1;
        std::string strVal = std::to_string(val);
        cadf::dom::DomNode ctorNode(val);
        BOOST_CHECK_EQUAL(strVal, ctorNode.operator std::string());
        BOOST_CHECK_EQUAL(val, ctorNode.operator long());
        BOOST_CHECK_EQUAL(false, ctorNode.operator bool());
        BOOST_CHECK_EQUAL(strVal.at(0), ctorNode.operator char());
        BOOST_CHECK_EQUAL(0, ctorNode.numChildren());
        BOOST_CHECK_EQUAL(0, ctorNode.numArrayElements());
        BOOST_CHECK_EQUAL(true, ctorNode.isLeaf());
        BOOST_CHECK_EQUAL(false, ctorNode.isString());
        BOOST_CHECK_EQUAL(false, ctorNode.isArray());
        BOOST_CHECK_EQUAL(false, ctorNode.isNull());

        val = (long) std::numeric_limits<int>::min() - 1;
        strVal = std::to_string(val);
        cadf::dom::DomNode assignNode = val;
        BOOST_CHECK_EQUAL(strVal, assignNode.operator std::string());
        BOOST_CHECK_EQUAL(val, assignNode.operator long());
        BOOST_CHECK_EQUAL(false, assignNode.operator bool());
        BOOST_CHECK_EQUAL('-', assignNode.operator char());
        BOOST_CHECK_EQUAL(0, assignNode.numChildren());
        BOOST_CHECK_EQUAL(0, assignNode.numArrayElements());
        BOOST_CHECK_EQUAL(true, assignNode.isLeaf());
        BOOST_CHECK_EQUAL(false, assignNode.isString());
        BOOST_CHECK_EQUAL(false, assignNode.isArray());
        BOOST_CHECK_EQUAL(false, assignNode.isNull());
    }

    BOOST_AUTO_TEST_CASE(DoubleNodeTest) {
        cadf::dom::DomNode ctorNode(-573.698);
        DomNodeTest::verifyNodeState(ctorNode, "-573.698", -573, -573.698, false, '-', 0, 0, true, false, false, false);

        cadf::dom::DomNode assignNode = -4.693;
        DomNodeTest::verifyNodeState(assignNode, "-4.693", -4, -4.693, false, '-', 0, 0, true, false, false, false);
    }

    BOOST_AUTO_TEST_CASE(BoolTrueNodeTest) {
        cadf::dom::DomNode ctorNode(true);
        DomNodeTest::verifyNodeState(ctorNode, "true", 0, 0, true, 't', 0, 0, true, false, false, false);

        cadf::dom::DomNode assignNode = true;
        DomNodeTest::verifyNodeState(assignNode, "true", 0, 0, true, 't', 0, 0, true, false, false, false);
    }

    BOOST_AUTO_TEST_CASE(BoolFalseNodeTest) {
        cadf::dom::DomNode ctorNode(false);
        DomNodeTest::verifyNodeState(ctorNode, "false", 0, 0, false, 'f', 0, 0, true, false, false, false);

        cadf::dom::DomNode assignNode = false;
        DomNodeTest::verifyNodeState(assignNode, "false", 0, 0, false, 'f', 0, 0, true, false, false, false);
    }

    BOOST_AUTO_TEST_CASE(CharNodeTest) {
        cadf::dom::DomNode ctorNode = '(';
        DomNodeTest::verifyNodeState(ctorNode, "(", 0, 0, false, '(', 0, 0, true, true, false, false);

        cadf::dom::DomNode assignNode = ')';
        DomNodeTest::verifyNodeState(assignNode, ")", 0, 0, false, ')', 0, 0, true, true, false, false);
    }

    BOOST_AUTO_TEST_CASE(ArrayNodeTest) {
        cadf::dom::DomNode assignNode;
        assignNode = {5, 4, 3, 2, 1};
        DomNodeTest::verifyNodeState(assignNode, "", 0, 0, false, '\0', 0, 5, true, false, true, false);

        int expected = 5;
        for (cadf::dom::DomNode::ArrayIterator it = assignNode.beginArray(); it != assignNode.endArray(); ++it)
            BOOST_CHECK_EQUAL(expected--, it->operator int());
    }

    BOOST_AUTO_TEST_CASE(StringArrayNodeTest) {
        cadf::dom::DomNode assignNode;
        assignNode = {"ABC", "DEF", "GHI"};
        DomNodeTest::verifyNodeState(assignNode, "", 0, 0, false, '\0', 0, 3, true, false, true, false);

        cadf::dom::DomNode::ArrayIterator it = assignNode.beginArray();
        BOOST_CHECK_EQUAL("ABC", (it++)->operator std::string());
        BOOST_CHECK_EQUAL("DEF", (it++)->operator std::string());
        BOOST_CHECK_EQUAL("GHI", (it++)->operator std::string());
        BOOST_CHECK(it == assignNode.endArray());
    }

    BOOST_AUTO_TEST_CASE(NamedValuesTreeTest) {
        cadf::dom::DomNode root;
        root["string"] = "This is a String";
        root["int"] = -58932;
        root["double"] = 75.329;
        root["bool"] = true;
        root["char"] = 'o';

        DomNodeTest::verifyNodeState(root, "", 0, 0, false, '\0', 5, 0, false, false, false, false);
        DomNodeTest::verifyNodeState(root["string"], "This is a String", 0, 0, false, 'T', 0, 0, true, true, false, false);
        DomNodeTest::verifyNodeState(root["int"], "-58932", -58932, -58932, false, '-', 0, 0, true, false, false, false);
        DomNodeTest::verifyNodeState(root["double"], "75.329", 75.329, 75.329, false, '7', 0, 0, true, false, false, false);
        DomNodeTest::verifyNodeState(root["bool"], "true", 0, 0, true, 't', 0, 0, true, false, false, false);
        DomNodeTest::verifyNodeState(root["char"], "o", 0, 0, false, 'o', 0, 0, true, true, false, false);
    }


    BOOST_AUTO_TEST_CASE(ChildNodeTest) {
        cadf::dom::DomNode root;
        cadf::dom::DomNode child;
        cadf::dom::DomNode value = 'e';
        child["value"] = value;
        root["child1"] = child;
        root["child2"]["value"] = 2;
        root["child3"]["value"] = "Third Child";
        root["child4"]["value"] = '?';

        DomNodeTest::verifyNodeState(root, "", 0, 0, false, '\0', 4, 0, false, false, false, false);
        DomNodeTest::verifyNodeState(root["child1"], "", 0, 0, false, '\0', 1, 0, false, false, false, false);
        DomNodeTest::verifyNodeState(root["child1"]["value"], "e", 0, 0, false, 'e', 0, 0, true, true, false, false);
        DomNodeTest::verifyNodeState(root["child2"], "", 0, 0, false, '\0', 1, 0, false, false, false, false);
        DomNodeTest::verifyNodeState(root["child2"]["value"], "2", 2, 2, false, '2', 0, 0, true, false, false, false);
        DomNodeTest::verifyNodeState(root["child3"], "", 0, 0, false, '\0', 1, 0, false, false, false, false);
        DomNodeTest::verifyNodeState(root["child3"]["value"], "Third Child", 0, 0, false, 'T', 0, 0, true, true, false, false);
        DomNodeTest::verifyNodeState(root["child4"], "", 0, 0, false, '\0', 1, 0, false, false, false, false);
        DomNodeTest::verifyNodeState(root["child4"]["value"], "?", 0, 0, false, '?', 0, 0, true, true, false, false);

        cadf::dom::DomNode::ChildIterator it = root.beginChildren();
        BOOST_CHECK_EQUAL("child1", it->first);
        BOOST_CHECK(&root["child1"] == &it->second);
        it++;
        BOOST_CHECK_EQUAL("child2", it->first);
        BOOST_CHECK(&root["child2"] == &it->second);
        it++;
        BOOST_CHECK_EQUAL("child3", it->first);
        BOOST_CHECK(&root["child3"] == &it->second);
        it++;
        BOOST_CHECK_EQUAL("child4", it->first);
        BOOST_CHECK(&root["child4"] == &it->second);
        it++;
        BOOST_CHECK(root.endChildren() == it);
    }

    BOOST_AUTO_TEST_CASE(ArraySubNodeTest) {
        cadf::dom::DomNode root;
        root["array"] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

        std::vector<double> dbls = {1.1, 2.2, 3.3, 4.4};
        root["doubleArray"] = dbls;

        DomNodeTest::verifyNodeState(root, "", 0, 0, false, '\0', 2, 0, false, false, false, false);

        DomNodeTest::verifyNodeState(root["array"], "", 0, 0, false, '\0', 0, 10, true, false, true, false);
        int expected = 1;
        for (cadf::dom::DomNode::ArrayIterator it = root["array"].beginArray(); it != root["array"].endArray(); ++it)
            BOOST_CHECK_EQUAL(expected++, it->operator int());

        DomNodeTest::verifyNodeState(root["doubleArray"], "", 0, 0, false, '\0', 0, 4, true, false, true, false);
        expected = 1;
        for (cadf::dom::DomNode::ArrayIterator it = root["doubleArray"].beginArray(); it != root["doubleArray"].endArray(); ++it) {
            BOOST_CHECK_EQUAL(expected + expected / 10.0, it->operator double());
            expected++;
        }
    }

    BOOST_AUTO_TEST_CASE(NullArrayNodeTest) {
        cadf::dom::DomNode root;
        cadf::dom::DomNode nullNode1;
        cadf::dom::DomNode nullNode2;
        cadf::dom::DomNode nullNode3;
        cadf::dom::DomNode nullNode4;
        root["nullArray"] = {nullNode1, nullNode2, nullNode3, nullNode4};

        DomNodeTest::verifyNodeState(root, "", 0, 0, false, '\0', 1, 0, false, false, false, false);
        DomNodeTest::verifyNodeState(root["nullArray"], "", 0, 0, false, '\0', 0, 4, true, false, true, false);

        for (cadf::dom::DomNode::ArrayIterator it = root["nullArray"].beginArray(); it != root["nullArray"].endArray(); ++it)
            DomNodeTest::verifyNodeState(*it, "", 0, 0, false, '\0', 0, 0, true, false, false, true);
    }

    BOOST_AUTO_TEST_CASE(BuildNodeHelperTest) {
        cadf::dom::DomNode node = cadf::dom::buildNode("key", "value");
        DomNodeTest::verifyNodeState(node, "", 0, 0, false, '\0', 1, 0, false, false, false, false);
        DomNodeTest::verifyNodeState(node["key"], "value", 0, 0, false, 'v', 0, 0, true, true, false, false);
    }

    BOOST_AUTO_TEST_CASE(BuildArrayNodeHelperTest) {
        cadf::dom::DomNode node = cadf::dom::buildNode("key", {"A", "B", "C", "D"});
        DomNodeTest::verifyNodeState(node, "", 0, 0, false, '\0', 1, 0, false, false, false, false);
        DomNodeTest::verifyNodeState(node["key"], "", 0, 0, false, '\0', 0, 4, true, false, true, false);

        cadf::dom::DomNode::ArrayIterator it = node["key"].beginArray();
        BOOST_CHECK_EQUAL("A", (it++)->operator std::string());
        BOOST_CHECK_EQUAL("B", (it++)->operator std::string());
        BOOST_CHECK_EQUAL("C", (it++)->operator std::string());
        BOOST_CHECK_EQUAL("D", (it++)->operator std::string());
        BOOST_CHECK(node["key"].endArray() == it);
    }

    BOOST_AUTO_TEST_CASE(ExtensiveTreeTest) {
        cadf::dom::DomNode root;
        root["1_1"] = "StringVal";
        root["1_2"] = 123;
        root["1_3"] = '_';
        root["1_4"]["1_4_1"] = 32.32;
        root["1_4"]["1_4_2"] = "SubValue";
        root["1_5"]["1_5_1"]["1_5_1_1"]["1_5_1_1_1"] = {'A', 'B', 'C'};
        root["1_6"];
        root["1_7"] = {cadf::dom::DomNode(), cadf::dom::DomNode()};
        root["1_8"] = {cadf::dom::DomNode({1, 2, 3}) , cadf::dom::DomNode({4, 5, 6})};
        root["1_9"] = {cadf::dom::buildNode("values", { "Hi", "There" }), cadf::dom::buildNode("values", { "See", "You" })};

        // Simple nodes
        DomNodeTest::verifyNodeState(root, "", 0, 0, false, '\0', 9, 0, false, false, false, false);
        DomNodeTest::verifyNodeState(root["1_1"], "StringVal", 0, 0, false, 'S', 0, 0, true, true, false, false);
        DomNodeTest::verifyNodeState(root["1_2"], "123", 123, 123, false, '1', 0, 0, true, false, false, false);
        DomNodeTest::verifyNodeState(root["1_3"], "_", 0, 0, false, '_', 0, 0, true, true, false, false);
        DomNodeTest::verifyNodeState(root["1_4"], "", 0, 0, false, '\0', 2, 0, false, false, false, false);
        DomNodeTest::verifyNodeState(root["1_4"]["1_4_1"], "32.32", 32, 32.32, false, '3', 0, 0, true, false, false, false);
        DomNodeTest::verifyNodeState(root["1_4"]["1_4_2"], "SubValue", 0, 0, false, 'S', 0, 0, true, true, false, false);
        DomNodeTest::verifyNodeState(root["1_6"], "", 0, 0, false, '\0', 0, 0, true, false, false, true);

        // The nested 1_5 nodes
        DomNodeTest::verifyNodeState(root["1_5"], "", 0, 0, false, '\0', 1, 0, false, false, false, false);
        DomNodeTest::verifyNodeState(root["1_5"]["1_5_1"], "", 0, 0, false, '\0', 1, 0, false, false, false, false);
        DomNodeTest::verifyNodeState(root["1_5"]["1_5_1"]["1_5_1_1"], "", 0, 0, false, '\0', 1, 0, false, false, false, false);
        DomNodeTest::verifyNodeState(root["1_5"]["1_5_1"]["1_5_1_1"]["1_5_1_1_1"], "", 0, 0, false, '\0', 0, 3, true, false, true, false);
        cadf::dom::DomNode::ArrayIterator it = root["1_5"]["1_5_1"]["1_5_1_1"]["1_5_1_1_1"].beginArray();
        BOOST_CHECK_EQUAL('A', (it++)->operator char());
        BOOST_CHECK_EQUAL('B', (it++)->operator char());
        BOOST_CHECK_EQUAL('C', (it++)->operator char());
        BOOST_CHECK(root["1_5"]["1_5_1"]["1_5_1_1"]["1_5_1_1_1"].endArray() == it);

        // 1_7 is an array of nulls
        DomNodeTest::verifyNodeState(root["1_7"], "", 0, 0, false, '\0', 0, 2, true, false, true, false);
        for (it = root["1_7"].beginArray(); it != root["1_7"].endArray(); ++it)
            BOOST_CHECK(it->isNull());

        // 1_8 is a nested array
        DomNodeTest::verifyNodeState(root["1_8"], "", 0, 0, false, '\0', 0, 2, true, false, true, false);
        int expected = 1;
        for (it = root["1_8"].beginArray(); it != root["1_8"].endArray(); ++it) {
            DomNodeTest::verifyNodeState(*it, "", 0, 0, false, '\0', 0, 3, true, false, true, false);
            for (cadf::dom::DomNode::ArrayIterator nested = it->beginArray(); nested != it->endArray(); ++nested)
                BOOST_CHECK_EQUAL(expected++, nested->operator int());
        }

        // 1_9 contains nodes with an array each
        DomNodeTest::verifyNodeState(root["1_9"], "", 0, 0, false, '\0', 0, 2, true, false, true, false);
        it = root["1_9"].beginArray();
        DomNodeTest::verifyNodeState(*it, "", 0, 0, false, '\0', 1, 0, false, false, false, false);
        DomNodeTest::verifyNodeState(it->operator[]("values"), "", 0, 0, false, '\0', 0, 2, true, false, true, false);
        cadf::dom::DomNode::ArrayIterator nested = it->operator[]("values").beginArray();
        BOOST_CHECK_EQUAL("Hi", (nested++)->operator std::string());
        BOOST_CHECK_EQUAL("There", (nested++)->operator std::string());
        BOOST_CHECK(it->operator[]("values").endArray() == nested);
        it++;
        DomNodeTest::verifyNodeState(it->operator[]("values"), "", 0, 0, false, '\0', 0, 2, true, false, true, false);
        nested = it->operator[]("values").beginArray();
        BOOST_CHECK_EQUAL("See", (nested++)->operator std::string());
        BOOST_CHECK_EQUAL("You", (nested++)->operator std::string());
        BOOST_CHECK(it->operator[]("values").endArray() == nested);
        it++;
        BOOST_CHECK(root["1_9"].endArray() == it);
    }

    BOOST_AUTO_TEST_SUITE_END()
