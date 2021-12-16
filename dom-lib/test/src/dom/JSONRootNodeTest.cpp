#define BOOST_TEST_DYN_LINK
#ifdef STAND_ALONE
#   define BOOST_TEST_MODULE Main
#endif
#include <boost/test/unit_test.hpp>
#include <fakeit.hpp>

#include "dom/JSONRootNode.h"

namespace JSONRootNodeTest {
    /*
     * Helper fixture for initializing and preparing all of the necessary mocks
     */
    struct SetupMocks {

            std::string VAL_NAME = "Value";
            std::string VAL_TO_STRING = "VALUE To String";
            std::string VAL_VALUE = "VALUE Value";

            SetupMocks() {
                fakeit::When(Method(mockValue, getName)).AlwaysReturn(VAL_NAME);
                fakeit::When(Method(mockValue, toString)).AlwaysReturn(VAL_TO_STRING);
                fakeit::When(Method(mockValue, valueToString)).AlwaysReturn(VAL_VALUE);
            }

            ~SetupMocks() {
                verifyAllMocksChecked();
            }

            void verifyAllMocksChecked() {
                fakeit::VerifyNoOtherInvocations(mockValue);
            }

            // Mocks to use for testing
            fakeit::Mock<camb::dom::JSONValue> mockValue;
    };

    /**
     * Helper fixture to initialize the LocalConnection instance that is to be used for the tests
     */
    struct TestFixture: public SetupMocks {

            TestFixture() : SetupMocks(), root() {
                BOOST_CHECK_EQUAL("", root.getName());
                BOOST_CHECK_EQUAL("{}", root.toString());

                // Verify the initialization of the class to be tested
                verifyAllMocksChecked();
            }

            // The class to test
            camb::dom::JSONRootNode root;
    };

    /**
     * Test class to be used to verify that the added memory is also cleaned up.
     */
    class MemTestNode: public camb::dom::JSONValue {
        public:
            static bool isInitialized;

            MemTestNode(): JSONValue("MemTestNode") {
                isInitialized = true;
            }

            ~MemTestNode() {
                isInitialized = false;
            }
    };

    bool MemTestNode::isInitialized = false;

    /**
     * Helper class that is to be used to expose the addToMemory() method of the root node.
     */
    class MemExposeRootNode: public camb::dom::JSONRootNode {
        public:
            void exposeAddToMemory(JSONValue* value) {
                JSONRootNode::addToMemory(value);
            }
    };
}

/**
 * Test suite for base JSONRootNode class
 */
BOOST_AUTO_TEST_SUITE(JSONRootNode_Test_Suite)

/**
 * Verify that the root node is creates as expected
 */
    BOOST_FIXTURE_TEST_CASE(CreateNewNodeTest, JSONRootNodeTest::TestFixture) {
        root.setValue(&mockValue.get());
        fakeit::Verify(Method(mockValue, getName)).Exactly(1);
        BOOST_CHECK_EQUAL("{" + VAL_TO_STRING + "}", root.toString());
        fakeit::Verify(Method(mockValue, toString)).Exactly(1);
    }

    /**
     * Verify that the root node cannot be added as a child
     */
    BOOST_FIXTURE_TEST_CASE(AddRootAsChildTest, JSONRootNodeTest::TestFixture) {
        camb::dom::JSONNode node("ChildNode");
        BOOST_REQUIRE_THROW(node.setValue(&root), camb::dom::JSONTreeException);
    }

    /**
     * Verify that added memory is also cleaned up on root destruction.
     */
    BOOST_AUTO_TEST_CASE(MemoryCleanupTest) {
        BOOST_CHECK_EQUAL(false, JSONRootNodeTest::MemTestNode::isInitialized);
        JSONRootNodeTest::MemExposeRootNode *root = new JSONRootNodeTest::MemExposeRootNode();
        root->exposeAddToMemory(new JSONRootNodeTest::MemTestNode());
        BOOST_CHECK_EQUAL(true, JSONRootNodeTest::MemTestNode::isInitialized);

        delete(root);
        BOOST_CHECK_EQUAL(false, JSONRootNodeTest::MemTestNode::isInitialized);
    }

    BOOST_AUTO_TEST_SUITE_END()
