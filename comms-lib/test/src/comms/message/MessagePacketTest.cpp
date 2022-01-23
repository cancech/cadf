#define BOOST_TEST_DYN_LINK
#ifdef STAND_ALONE
#   define BOOST_TEST_MODULE Main
#endif
#include <boost/test/unit_test.hpp>

#include "comms/message/MessagePacket.h"
#include "TestMessage.h"

#include <fakeit.hpp>

// Helper classes for the MessageRegistryTest
namespace MessagePacketTest {

    /*
     * Helper fixture for initializing and preparing all of the necessary mocks
     */
    struct SetupMocks {

            SetupMocks() {
            }

            ~SetupMocks() {
                verifyAllMocksChecked();
            }

            void verifyAllMocksChecked() {
                fakeit::VerifyNoOtherInvocations(mockMessage1, mockMessage2, mockMessage3);
            }

            // Mocks to use for testing
            fakeit::Mock<cadf::comms::IMessage> mockMessage1;
            fakeit::Mock<cadf::comms::IMessage> mockMessage2;
            fakeit::Mock<cadf::comms::IMessage> mockMessage3;
            fakeit::Mock<cadf::comms::IMessage> mockMessage4;
            fakeit::Mock<cadf::comms::IMessage> mockMessage5;
    };

    void verifyMessageDetails(int expectedType, int expectedInstance, cadf::comms::IMessage *expectedMsg, bool expectedGlobalBroadcast, bool expectedRecipientBroadcast, cadf::comms::MessagePacket &actual) {
        BOOST_CHECK_EQUAL(expectedType, actual.getRecipientType());
        BOOST_CHECK_EQUAL(expectedInstance, actual.getRecipientInstance());
        BOOST_CHECK_EQUAL(expectedMsg, actual.getMessage());

        BOOST_CHECK_EQUAL(expectedGlobalBroadcast, actual.isTypeBroadcast());
        BOOST_CHECK_EQUAL(expectedRecipientBroadcast, actual.isInstanceBroadcast());
    }
}

/**
 * Test suite for the unit testing of the MessagePacket
 */
BOOST_AUTO_TEST_SUITE(MessagePacket_Test_Suite)

/**
 * Verify that when all values are passed into the packet constructor, it will properly work
 */
    BOOST_FIXTURE_TEST_CASE(CreateMessageTest, MessagePacketTest::SetupMocks) {
        cadf::comms::MessagePacket p1(&mockMessage1.get(), 123, 456);
        MessagePacketTest::verifyMessageDetails(123, 456, &mockMessage1.get(), false, false, p1);

        cadf::comms::MessagePacket p2(&mockMessage2.get(), 789, 101112);
        MessagePacketTest::verifyMessageDetails(789, 101112, &mockMessage2.get(), false, false, p2);

        cadf::comms::MessagePacket p3(&mockMessage3.get(), 13579, 24680);
        MessagePacketTest::verifyMessageDetails(13579, 24680, &mockMessage3.get(), false, false, p3);

        cadf::comms::MessagePacket p4(&mockMessage4.get(), 123, -123);
        MessagePacketTest::verifyMessageDetails(123, -123, &mockMessage4.get(), false, true, p4);

        cadf::comms::MessagePacket p5(&mockMessage5.get(), -123, 123);
        MessagePacketTest::verifyMessageDetails(-123, 123, &mockMessage5.get(), true, false, p5);
    }

    /**
     * Verify that the packet can be cloned
     */
    BOOST_FIXTURE_TEST_CASE(CloneMessageTest, MessagePacketTest::SetupMocks) {
        // Create original
        fakeit::When(Method(mockMessage1, clone)).AlwaysDo([]() { return new TestMessage1(); });
        cadf::comms::MessagePacket packet(&mockMessage1.get(), 101, 202);

        // Clone it
        cadf::comms::MessagePacket *clone = packet.clone();

        // Verify the resulting clone
        fakeit::Verify(Method(mockMessage1, clone)).Once();
        BOOST_CHECK(NULL != clone);
        BOOST_CHECK(NULL != clone->getMessage());
        BOOST_CHECK(&mockMessage1.get() != clone->getMessage());
        BOOST_CHECK_EQUAL("TestMessage1", clone->getMessage()->getType());
        BOOST_CHECK_EQUAL(101, clone->getRecipientType());
        BOOST_CHECK_EQUAL(202, clone->getRecipientInstance());
        delete(clone);
    }

    BOOST_AUTO_TEST_SUITE_END()
