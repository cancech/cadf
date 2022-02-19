
#define BOOST_TEST_DYN_LINK
#ifdef STAND_ALONE
#   define BOOST_TEST_MODULE Main
#endif
#include <boost/test/unit_test.hpp>

#include "TestData.h"
#include "TestMessage.h"
#include "comms/network/serializer/dom/Serializer.h"

/**
 * Test suite for the Serialization functions
 */
BOOST_AUTO_TEST_SUITE(SerializerJSON_Test_Suite)

    /**
     * Verify that the size of the data is properly determined.
     */
    BOOST_AUTO_TEST_CASE(TestSerializeDeserializeDataExactSize) {
        TestData data { 123, 1.23 };
        TestMessage1 msg(data);

        cadf::comms::dom::Serializer<TestData> serializer(cadf::dom::json::JsonConverter::instance(), &msg, 1, 2);
        BOOST_CHECK_EQUAL(81, serializer.getSize());
        cadf::comms::OutputBuffer outBuffer(serializer.getSize());
        serializer.serialize(&outBuffer);
        BOOST_CHECK_EQUAL("{\"data\":{\"val1\":123,\"val2\":1.23},\"instance\":2,\"message\":\"TestMessage1\",\"type\":1}", outBuffer.getData());

        cadf::comms::InputBuffer inBuffer(outBuffer.getData(), outBuffer.getSize());
        cadf::comms::dom::Deserializer deserializer(cadf::dom::json::JsonConverter::instance(), &inBuffer);
        BOOST_CHECK_EQUAL("TestMessage1", deserializer.getMessageType());
        BOOST_CHECK_EQUAL(1, deserializer.getRecipientType());
        BOOST_CHECK_EQUAL(2, deserializer.getRecipientInstance());

        TestData newData = deserializer.getData<TestData>();
        BOOST_CHECK_EQUAL(data, newData);
    }

    /**
     * Verify that the size of the data is properly determined.
     */
    BOOST_AUTO_TEST_CASE(TestSerializeDeserializeDataLargeBufferSize) {
        TestData data { 456, 0.123 };
        TestMessage2 msg(data);
        cadf::comms::OutputBuffer outBuffer(1000);

        cadf::comms::dom::Serializer<TestData> serializer(cadf::dom::json::JsonConverter::instance(), &msg, 3, 4);
        serializer.serialize(&outBuffer);
        BOOST_CHECK_EQUAL("{\"data\":{\"val1\":456,\"val2\":0.123},\"instance\":4,\"message\":\"TestMessage2\",\"type\":3}", outBuffer.getData());

        cadf::comms::InputBuffer inBuffer(outBuffer.getData(), outBuffer.getSize());
        cadf::comms::dom::Deserializer deserializer(cadf::dom::json::JsonConverter::instance(), &inBuffer);
        BOOST_CHECK_EQUAL("TestMessage2", deserializer.getMessageType());
        BOOST_CHECK_EQUAL(3, deserializer.getRecipientType());
        BOOST_CHECK_EQUAL(4, deserializer.getRecipientInstance());

        TestData newData = deserializer.getData<TestData>();
        BOOST_CHECK_EQUAL(data, newData);
    }

    /**
     * Verify that a message can be serialized and deserialized when the protocol is employed
     */
    BOOST_AUTO_TEST_CASE(TestProtocolSerializeDeserialize) {
        TestData data { 987, 4.321 };
        TestMessage3 msg(data);

        cadf::comms::ISerializerFactory *serializerFactory = cadf::comms::dom::json::JSONProtocol::createSerializerFactory(&msg);
        cadf::comms::ISerializer *serializer = serializerFactory->buildSerializer(&msg, 5, 6);
        cadf::comms::OutputBuffer out(serializer->getSize());
        serializer->serialize(&out);
        BOOST_CHECK_EQUAL("{\"data\":{\"val1\":987,\"val2\":4.321},\"instance\":6,\"message\":\"TestMessage3\",\"type\":5}", out.getData());

        cadf::comms::InputBuffer in(out.getData(), out.getSize());
        cadf::comms::IDeserializer *deserializer = cadf::comms::dom::json::JSONProtocol::createDeserializer(&in);
        BOOST_CHECK_EQUAL("TestMessage3", deserializer->getMessageType());
        BOOST_CHECK_EQUAL(5, deserializer->getRecipientType());
        BOOST_CHECK_EQUAL(6, deserializer->getRecipientInstance());
        TestMessage3 received;
        serializerFactory->deserializeTo(&received, deserializer);
        BOOST_CHECK_EQUAL(data, received.getData());

        delete(serializer);
        delete(deserializer);
        delete(serializerFactory);
    }

    BOOST_AUTO_TEST_SUITE_END()
