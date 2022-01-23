#define BOOST_TEST_DYN_LINK
#ifdef STAND_ALONE
#   define BOOST_TEST_MODULE Main
#endif
#include <boost/test/unit_test.hpp>

#include "comms/message/Message.h"
#include "comms/network/Buffer.h"
#include "comms/network/serializer/binary/Serializer.h"
#include "TestData.h"

BOOST_AUTO_TEST_SUITE(Message_Test_Suite)

/**
 * Verify that can create a message properly.
 */
    BOOST_AUTO_TEST_CASE(MessageCreationTest) {
        TestData data { 123, 456 };

        cadf::comms::DataMessage<TestData> msg("TestDataMessage", data);
        BOOST_CHECK_EQUAL("TestDataMessage", msg.getType());
        BOOST_CHECK_EQUAL(data, msg.getData());
    }

    /**
     * Verify that can clone a message
     */
    BOOST_AUTO_TEST_CASE(MessageCloneTest) {
        TestData data { 123, 456 };

        cadf::comms::DataMessage<TestData> msg("TestDataMessage", data);
        cadf::comms::DataMessage<TestData> *clone = (cadf::comms::DataMessage<TestData>*) msg.clone();
        BOOST_CHECK(&msg != clone);
        BOOST_CHECK_EQUAL("TestDataMessage", clone->getType());
        BOOST_CHECK_EQUAL(data, clone->getData());
        delete (clone);
    }

    /**
     * Verify that can update the data in the message
     */
    BOOST_AUTO_TEST_CASE(UpdateDataTest) {
        TestData orig { 123, 456 };
        TestData updated { 456, 123 };

        cadf::comms::DataMessage<TestData> msg("TestDataMessage", orig);
        BOOST_CHECK_EQUAL(orig, msg.getData());

        msg.setData(updated);
        BOOST_CHECK_EQUAL(updated, msg.getData());
    }

    /**
     * Verify that can serialize and deserialize a message
     */
    BOOST_AUTO_TEST_CASE(SerializeDeserializeTest) {
        // Prepare the original message
        TestData data { 456, 9.87 };
        cadf::comms::DataMessage<TestData> msg("TestDataMessage", data);

        // Serialize

        cadf::comms::OutputBuffer out(cadf::comms::binary::DataSerializer<TestData>::sizeOf(data));
        cadf::comms::binary::DataSerializer<TestData>::serialize(data, &out);

        // Deserialize it
        cadf::comms::InputBuffer in(out.getData(), out.getSize());
        cadf::comms::DataMessage<TestData> copy("TestDataMessage", cadf::comms::binary::DataSerializer<TestData>::deserialize(&in));

        BOOST_CHECK_EQUAL(data, copy.getData());
    }

    BOOST_AUTO_TEST_SUITE_END()
