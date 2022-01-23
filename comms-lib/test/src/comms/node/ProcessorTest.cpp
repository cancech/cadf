
#define BOOST_TEST_DYN_LINK
#ifdef STAND_ALONE
#   define BOOST_TEST_MODULE Main
#endif
#include <boost/test/unit_test.hpp>

#include "comms/node/Processor.h"
#include <TestMessage.h>

/**
 * Message receiver implementation which is to be used for the test.
 */
class TestMessageProcessor: public cadf::comms::MessageProcessor<TestMessage1> {

public:
    /**
     * CTOR
     */
    TestMessageProcessor(): m_receivedMsg(NULL) {}

    /**
     * Called by the MessageReceiver parent when a message is to be processed.
     * In this case just keep track of the pointer so that it can be checked
     * in the test.
     */
    void processMessage(const TestMessage1* msg) {
        m_receivedMsg = msg;
    }

    /**
     * Helper method to simplify the checking and resetting of the receiver. Makes
     * sure that the correct message pointer is stored, and resets it for the next
     * test.
     */
    void checkIsMsgReceived(TestMessage1* msg) {
        BOOST_CHECK_EQUAL(msg, m_receivedMsg);
        m_receivedMsg = NULL;
    }

private:
    /** Pointer to the last message that was received */
    const TestMessage1* m_receivedMsg;
};

/**
 * Test case for verifying the MessageReceiver via the TestMessageReceiver above
 */
BOOST_AUTO_TEST_SUITE(MessageProcessor_Test_Suite)

/**
 * Verify that the receiver can be properly initialized
 */
BOOST_AUTO_TEST_CASE(TestInitialization) {
    TestMessageProcessor receiver;
    BOOST_CHECK_EQUAL("TestMessage1", receiver.getType());
    receiver.checkIsMsgReceived(NULL);
}

/**
 * Verify that the receiver can properly receive messages of the valid type.
 */
BOOST_AUTO_TEST_CASE(TestReceiveValidMessage) {
    TestMessageProcessor receiver;

    TestMessage1 msg1;
    TestMessage1 msg2;
    TestMessage1 msg3;

    receiver.checkIsMsgReceived(NULL);
    receiver.process(&msg1);
    receiver.checkIsMsgReceived(&msg1);
    receiver.process(&msg3);
    receiver.checkIsMsgReceived(&msg3);
    receiver.process(&msg2);
    receiver.checkIsMsgReceived(&msg2);
}

/**
 * Verify that the receiver properly generates an exception when an invalid message
 * type is received.
 */
BOOST_AUTO_TEST_CASE(TestReceiveInvalidMessage) {
    TestMessageProcessor receiver;

    TestMessage2 msg1;
    TestMessage3 msg2;
    TestMessage1 msg3;

    // Invalid messages generate exceptions
    receiver.checkIsMsgReceived(NULL);
    BOOST_REQUIRE_THROW(receiver.process(&msg1), cadf::comms::InvalidMessageTypeException);
    receiver.checkIsMsgReceived(NULL);
    BOOST_REQUIRE_THROW(receiver.process(&msg2), cadf::comms::InvalidMessageTypeException);
    receiver.checkIsMsgReceived(NULL);

    // And can still receive a valid message afterwards
    receiver.process(&msg3);
    receiver.checkIsMsgReceived(&msg3);
}

BOOST_AUTO_TEST_SUITE_END()
