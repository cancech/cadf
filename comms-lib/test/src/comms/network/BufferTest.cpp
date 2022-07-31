
#define BOOST_TEST_DYN_LINK
#ifdef STAND_ALONE
#   define BOOST_TEST_MODULE Main
#endif
#include <boost/test/unit_test.hpp>

#include "comms/network/Buffer.h"
#include "comms/network/serializer/binary/Serializer.h"
#include "TestMessage.h"

/**
 * Test suite for the Buffers
 */
BOOST_AUTO_TEST_SUITE(NetworkBuffer_Test_Suite)

/**
 * Verify that can serialize and deserialize some values to/from the buffer
 */
    BOOST_AUTO_TEST_CASE(TestDataBufferTest) {
        TestData data = { 123, 4.321 };

        cadf::comms::OutputBuffer out(cadf::comms::binary::DataSerializer<TestData>::sizeOf(data) + sizeof(int));
        BOOST_CHECK_EQUAL(0, out.getDataSize());
        BOOST_CHECK_EQUAL(cadf::comms::binary::DataSerializer<TestData>::sizeOf(data) + sizeof(int), out.getTotalSize());
        cadf::comms::binary::serializeData(data, &out);
        BOOST_CHECK_EQUAL(cadf::comms::binary::DataSerializer<TestData>::sizeOf(data), out.getDataSize());
        int* extra = new int (10101);
        out.append(extra, sizeof(int));
        BOOST_CHECK_EQUAL(cadf::comms::binary::DataSerializer<TestData>::sizeOf(data) + sizeof(int), out.getDataSize());

        cadf::comms::InputBuffer in(out.getData(), out.getDataSize());
        BOOST_CHECK_EQUAL(cadf::comms::binary::DataSerializer<TestData>::sizeOf(data) + sizeof(int), in.getDataSize());
        BOOST_CHECK_EQUAL(cadf::comms::binary::DataSerializer<TestData>::sizeOf(data) + sizeof(int), in.getTotalSize());
        TestData copy = cadf::comms::binary::deserializeData<TestData>(&in);
        int* copyExtra = in.retrieveNext<int*>(sizeof(int));

        BOOST_CHECK_EQUAL(data.val1, copy.val1);
        BOOST_CHECK_EQUAL(data.val2, copy.val2);
        BOOST_CHECK(extra != copyExtra);
        BOOST_CHECK_EQUAL(*extra, *copyExtra);
        delete(extra);
        delete(copyExtra);
    }

    /**
     * Verify that can serialize and deserialize a message
     */
    BOOST_AUTO_TEST_CASE(TestMessageBufferTest) {
        TestData data = { 789, 9.87 };
        TestMessage1 msg(data);

        cadf::comms::OutputBuffer out(cadf::comms::binary::DataSerializer<TestData>::sizeOf(data));
        cadf::comms::binary::DataSerializer<TestData>::serialize(data, &out);

        cadf::comms::InputBuffer in(out.getData(), out.getDataSize());
        TestMessage1 copy(cadf::comms::binary::DataSerializer<TestData>::deserialize(&in));
        BOOST_CHECK_EQUAL(data, copy.getData());
    }

    /**
     * Verify that an exception is throw when trying to overflow the Output message buffer
     */
    BOOST_AUTO_TEST_CASE(SerializeBufferOverflowTest) {
        TestData data = { 123, 4.321 };

        cadf::comms::OutputBuffer out(cadf::comms::binary::DataSerializer<TestData>::sizeOf(data));
        BOOST_CHECK_EQUAL(0, out.getDataSize());
        BOOST_CHECK_EQUAL(cadf::comms::binary::DataSerializer<TestData>::sizeOf(data), out.getTotalSize());
        cadf::comms::binary::DataSerializer<TestData>::serialize(data, &out);
        BOOST_CHECK_EQUAL(cadf::comms::binary::DataSerializer<TestData>::sizeOf(data), out.getDataSize());
        BOOST_REQUIRE_THROW(out.append('c', 1), cadf::comms::BufferOverflowException);
    }

    /**
     * Verify that an exception is throw when trying to overflow the Input message buffer
     */
    BOOST_AUTO_TEST_CASE(DeserializeBufferOverflowTest) {
        TestData data = { 123, 4.321 };

        cadf::comms::OutputBuffer out(cadf::comms::binary::DataSerializer<TestData>::sizeOf(data));
        BOOST_CHECK_EQUAL(0, out.getDataSize());
        BOOST_CHECK_EQUAL(cadf::comms::binary::DataSerializer<TestData>::sizeOf(data), out.getTotalSize());
        cadf::comms::binary::DataSerializer<TestData>::serialize(data, &out);
        BOOST_CHECK_EQUAL(cadf::comms::binary::DataSerializer<TestData>::sizeOf(data), out.getDataSize());

        cadf::comms::InputBuffer in(out.getData(), out.getDataSize());
        TestData copy = cadf::comms::binary::DataSerializer<TestData>::deserialize(&in);
        BOOST_CHECK_EQUAL(data.val1, copy.val1);
        BOOST_CHECK_EQUAL(data.val2, copy.val2);
        BOOST_REQUIRE_THROW(in.retrieveNext<char>(1), cadf::comms::BufferOverflowException);
    }

    /**
     * Verify that input buffer is only the size of the data
     */
    BOOST_AUTO_TEST_CASE(TestInputBufferMatchesDataSize) {
        TestData data = { 1, 2 };
        TestMessage1 msg(data);

        // Make sure that there is not data, but the buffer is the right size
        cadf::comms::OutputBuffer out(100);
        BOOST_CHECK_EQUAL(0, out.getDataSize());
        BOOST_CHECK_EQUAL(100, out.getTotalSize());
        // Add data, and the correct amount of data is stored, with the buffer staying the expected size
        cadf::comms::binary::DataSerializer<TestData>::serialize(data, &out);
        BOOST_CHECK_EQUAL(cadf::comms::binary::DataSerializer<TestData>::sizeOf(data), out.getDataSize());
        BOOST_CHECK_EQUAL(100, out.getTotalSize());

        // Copy the data to an input buffer, data sizes in the output unchanged, input buffer only big enough to contain the data
        cadf::comms::InputBuffer in(out.getData(), out.getDataSize());
        BOOST_CHECK_EQUAL(cadf::comms::binary::DataSerializer<TestData>::sizeOf(data), out.getDataSize());
        BOOST_CHECK_EQUAL(100, out.getTotalSize());
        BOOST_CHECK_EQUAL(cadf::comms::binary::DataSerializer<TestData>::sizeOf(data), in.getDataSize());
        BOOST_CHECK_EQUAL(cadf::comms::binary::DataSerializer<TestData>::sizeOf(data), in.getTotalSize());

        // Make sure that the correct data is stored
        TestMessage1 copy(cadf::comms::binary::DataSerializer<TestData>::deserialize(&in));
        BOOST_CHECK_EQUAL(data, copy.getData());
    }

    BOOST_AUTO_TEST_SUITE_END()
