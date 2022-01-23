
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
        BOOST_CHECK_EQUAL(cadf::comms::binary::DataSerializer<TestData>::sizeOf(data) + sizeof(int), out.getSize());
        cadf::comms::binary::serializeData(data, &out);
        int* extra = new int (10101);
        out.append(extra, sizeof(int));

        cadf::comms::InputBuffer in(out.getData(), out.getSize());
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
     * Verify that can serialize and deserialize an message
     */
    BOOST_AUTO_TEST_CASE(TestMessageBufferTest) {
        TestData data = { 789, 9.87 };
        TestMessage1 msg(data);

        cadf::comms::OutputBuffer out(cadf::comms::binary::DataSerializer<TestData>::sizeOf(data));
        cadf::comms::binary::DataSerializer<TestData>::serialize(data, &out);

        cadf::comms::InputBuffer in(out.getData(), out.getSize());
        TestMessage1 copy(cadf::comms::binary::DataSerializer<TestData>::deserialize(&in));
        BOOST_CHECK_EQUAL(data, copy.getData());
    }

    /**
     * Verify that an exception is throw when trying to overflow the Output message buffer
     */
    BOOST_AUTO_TEST_CASE(SerializeBufferOverflowTest) {
        TestData data = { 123, 4.321 };

        cadf::comms::OutputBuffer out(cadf::comms::binary::DataSerializer<TestData>::sizeOf(data));
        BOOST_CHECK_EQUAL(cadf::comms::binary::DataSerializer<TestData>::sizeOf(data), out.getSize());
        cadf::comms::binary::DataSerializer<TestData>::serialize(data, &out);
        BOOST_REQUIRE_THROW(out.append('c', 1), cadf::comms::BufferOverflowException);
    }

    /**
     * Verify that an exception is throw when trying to overflow the Input message buffer
     */
    BOOST_AUTO_TEST_CASE(DeserializeBufferOverflowTest) {
        TestData data = { 123, 4.321 };

        cadf::comms::OutputBuffer out(cadf::comms::binary::DataSerializer<TestData>::sizeOf(data));
        BOOST_CHECK_EQUAL(cadf::comms::binary::DataSerializer<TestData>::sizeOf(data), out.getSize());
        cadf::comms::binary::DataSerializer<TestData>::serialize(data, &out);

        cadf::comms::InputBuffer in(out.getData(), out.getSize());
        TestData copy = cadf::comms::binary::DataSerializer<TestData>::deserialize(&in);
        BOOST_CHECK_EQUAL(data.val1, copy.val1);
        BOOST_CHECK_EQUAL(data.val2, copy.val2);
        BOOST_REQUIRE_THROW(in.retrieveNext<char>(1), cadf::comms::BufferOverflowException);
    }

    BOOST_AUTO_TEST_SUITE_END()
