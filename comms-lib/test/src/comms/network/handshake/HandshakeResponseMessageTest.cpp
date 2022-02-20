
#define BOOST_TEST_DYN_LINK
#ifdef STAND_ALONE
#   define BOOST_TEST_MODULE Main
#endif
#include <boost/test/unit_test.hpp>
#include <sstream>

#include "comms/network/handshake/HandshakeResponseMessage.h"
#include "comms/network/serializer/binary/Serializer.h"
#include "comms/network/serializer/dom/BaseSerializer.h"

/**
 * Ensure the correct functionality of the HandshakeResponseV1Message
 */
BOOST_AUTO_TEST_SUITE(HandshakeResponseMessageV1_Test_Suite)

/**
 * Verify that the data can be properly compared
 */
    BOOST_AUTO_TEST_CASE(HandshakeResponseDataV1EqualTest) {
        BOOST_CHECK_EQUAL(cadf::comms::HandshakeResponseDataV1( { 1, 2 }), cadf::comms::HandshakeResponseDataV1( { 1, 2 }));
        BOOST_CHECK_EQUAL(cadf::comms::HandshakeResponseDataV1( { 456, 1 }), cadf::comms::HandshakeResponseDataV1( { 456, 1 }));
        BOOST_CHECK_EQUAL(cadf::comms::HandshakeResponseDataV1( { 1934, -123 }), cadf::comms::HandshakeResponseDataV1( { 1934, -123 }));

        BOOST_CHECK_EQUAL(false, cadf::comms::HandshakeResponseDataV1( { 1, 1 }) == cadf::comms::HandshakeResponseDataV1( { 2, 1 }));
        BOOST_CHECK_EQUAL(false, cadf::comms::HandshakeResponseDataV1( { 1, 1 }) == cadf::comms::HandshakeResponseDataV1( { 1, 2 }));
        BOOST_CHECK_EQUAL(false, cadf::comms::HandshakeResponseDataV1( { 123, 2345 }) == cadf::comms::HandshakeResponseDataV1( { 987, 1235 }));
    }

    /**
     * Verify that the data can be properly streamed
     */
    BOOST_AUTO_TEST_CASE(HandshakeResponseDataV1StreamTest) {
        std::stringstream ss;
        ss << cadf::comms::HandshakeResponseDataV1( { 341, 74 });
        BOOST_CHECK_EQUAL("[ clientType = 341, clientInstance = 74 ]", ss.str());

        ss.str("");
        ss << cadf::comms::HandshakeResponseDataV1( { 5, 8 });
        BOOST_CHECK_EQUAL("[ clientType = 5, clientInstance = 8 ]", ss.str());

        ss.str("");
        ss << cadf::comms::HandshakeResponseDataV1( { 89, 89 });
        BOOST_CHECK_EQUAL("[ clientType = 89, clientInstance = 89 ]", ss.str());
    }

    /**
     * Verify that the message can be initialized
     */
    BOOST_AUTO_TEST_CASE(HandshakeResponseMessageV1InitTest) {
        cadf::comms::HandshakeResponseMessageV1 defaultMsg;
        BOOST_CHECK_EQUAL(0, defaultMsg.getData().clientType);
        BOOST_CHECK_EQUAL(0, defaultMsg.getData().clientInstance);
        BOOST_CHECK_EQUAL("HandshakeResponseMessageV1", defaultMsg.getType());

        cadf::comms::HandshakeResponseMessageV1 customMsg({574, 394});
        BOOST_CHECK_EQUAL(574, customMsg.getData().clientType);
        BOOST_CHECK_EQUAL(394, customMsg.getData().clientInstance);
        BOOST_CHECK_EQUAL("HandshakeResponseMessageV1", customMsg.getType());
    }

    /**
     * Verify that the message can be copied
     */
    BOOST_AUTO_TEST_CASE(HandshakeResponseMessageV1CopyTest) {
        cadf::comms::HandshakeResponseMessageV1 orig({987, 25});
        cadf::comms::AbstractDataMessage<cadf::comms::HandshakeResponseDataV1> *copy = orig.newInstance();
        BOOST_CHECK(&orig != copy);
        BOOST_CHECK_EQUAL(orig.getType(), copy->getType());
        BOOST_CHECK_EQUAL(orig.getData().clientType, copy->getData().clientType);
        BOOST_CHECK_EQUAL(orig.getData().clientInstance, copy->getData().clientInstance);
        delete(copy);
    }

    /**
     * Verify that the message can be serialized and deserialized from binary
     */
    BOOST_AUTO_TEST_CASE(HandshakeResponseV1BinarySerializationTest) {
        cadf::comms::HandshakeResponseDataV1 data({837, 9873});
        BOOST_CHECK_EQUAL(sizeof(int) * 2, cadf::comms::binary::sizeOfData(data));

        cadf::comms::OutputBuffer out(sizeof(int) * 2);
        cadf::comms::binary::serializeData(data, &out);

        cadf::comms::InputBuffer in(out.getData(), out.getSize());
        BOOST_CHECK_EQUAL(data, cadf::comms::binary::deserializeData<cadf::comms::HandshakeResponseDataV1>(&in));
    }

    /**
     * Verify that the message can be serialized and deserialized from JSON
     */
    BOOST_AUTO_TEST_CASE(HandshakeCompleteJSONSerializationTest) {
        cadf::comms::HandshakeResponseDataV1 data({457, 687});
        cadf::comms::HandshakeResponseMessageV1 msg(data);

        std::string expectedDataJson = "{\"clientInstance\":687,\"clientType\":457}";
        std::string expectedJson = "{\"data\":" + expectedDataJson + ",\"instance\":643,\"message\":\"HandshakeResponseMessageV1\",\"type\":987}";

        cadf::comms::OutputBuffer out(expectedJson.size() + 1);
        cadf::comms::dom::Serializer<cadf::comms::HandshakeResponseDataV1> serializer(cadf::dom::json::JsonConverter::instance(), &msg, 987, 643);
        serializer.serialize(&out);
        BOOST_CHECK_EQUAL(expectedJson, out.getData());

        cadf::comms::InputBuffer in(out.getData(), out.getSize());
        cadf::comms::dom::Deserializer deserializer(cadf::dom::json::JsonConverter::instance(), &in);
        BOOST_CHECK_EQUAL("HandshakeResponseMessageV1", deserializer.getMessageType());
        BOOST_CHECK_EQUAL(data, deserializer.getData<cadf::comms::HandshakeResponseDataV1>());
    }

    BOOST_AUTO_TEST_SUITE_END()
