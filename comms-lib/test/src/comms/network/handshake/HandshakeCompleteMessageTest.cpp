
#define BOOST_TEST_DYN_LINK
#ifdef STAND_ALONE
#   define BOOST_TEST_MODULE Main
#endif
#include <boost/test/unit_test.hpp>
#include <sstream>

#include "comms/network/handshake/HandshakeCompleteMessage.h"
#include "comms/network/serializer/binary/Serializer.h"
#include "comms/network/serializer/dom/Serializer.h"

/**
 * Ensure the correct functionality of the HandshakeCompleteMessage
 */
BOOST_AUTO_TEST_SUITE(HandshakeCompleteMessage_Test_Suite)

/**
 * Verify that the data can be properly compared
 */
    BOOST_AUTO_TEST_CASE(HandshakeCompleteDataEqualTest) {
        BOOST_CHECK_EQUAL(cadf::comms::HandshakeCompleteData( { 1 }), cadf::comms::HandshakeCompleteData( { 1 }));
        BOOST_CHECK_EQUAL(cadf::comms::HandshakeCompleteData( { 456 }), cadf::comms::HandshakeCompleteData( { 456 }));
        BOOST_CHECK_EQUAL(cadf::comms::HandshakeCompleteData( { 1934 }), cadf::comms::HandshakeCompleteData( { 1934 }));

        BOOST_CHECK_EQUAL(false, cadf::comms::HandshakeCompleteData( { 1 }) == cadf::comms::HandshakeCompleteData( { 2 }));
        BOOST_CHECK_EQUAL(false, cadf::comms::HandshakeCompleteData( { 123 }) == cadf::comms::HandshakeCompleteData( { 234 }));
        BOOST_CHECK_EQUAL(false, cadf::comms::HandshakeCompleteData( { 1934 }) == cadf::comms::HandshakeCompleteData( { 4391 }));
    }

    /**
     * Verify that the data can be properly streamed
     */
    BOOST_AUTO_TEST_CASE(HandshakeCompleteDataStreamTest) {
        std::stringstream ss;
        ss << cadf::comms::HandshakeCompleteData( { 4521 });
        BOOST_CHECK_EQUAL("[ version = 4521 ]", ss.str());

        ss.str("");
        ss << cadf::comms::HandshakeCompleteData( { 5678 });
        BOOST_CHECK_EQUAL("[ version = 5678 ]", ss.str());

        ss.str("");
        ss << cadf::comms::HandshakeCompleteData( { 0 });
        BOOST_CHECK_EQUAL("[ version = 0 ]", ss.str());
    }

    /**
     * Verify that the message can be initialized
     */
    BOOST_AUTO_TEST_CASE(HandshakeCompleteMessageInitTest) {
        cadf::comms::HandshakeCompleteMessage defaultMsg;
        BOOST_CHECK_EQUAL(0, defaultMsg.getData().version);
        BOOST_CHECK_EQUAL("HandshakeCompleteMessage", defaultMsg.getType());

        cadf::comms::HandshakeCompleteMessage customMsg(cadf::comms::HandshakeCompleteData({654}));
        BOOST_CHECK_EQUAL(654, customMsg.getData().version);
        BOOST_CHECK_EQUAL("HandshakeCompleteMessage", customMsg.getType());
    }

    /**
     * Verify that the message can be copied
     */
    BOOST_AUTO_TEST_CASE(HandshakeCompleteMessageCopyTest) {
        cadf::comms::HandshakeCompleteMessage orig(cadf::comms::HandshakeCompleteData({984}));
        cadf::comms::AbstractDataMessage<cadf::comms::HandshakeCompleteData> *copy = orig.newInstance();
        BOOST_CHECK(&orig != copy);
        BOOST_CHECK_EQUAL(orig.getType(), copy->getType());
        BOOST_CHECK_EQUAL(orig.getData().version, copy->getData().version);
        delete(copy);
    }

    /**
     * Verify that the message can be serialized and deserialized from binary
     */
    BOOST_AUTO_TEST_CASE(HandshakeCompleteBinarySerializationTest) {
        cadf::comms::HandshakeCompleteData data({837});
        BOOST_CHECK_EQUAL(sizeof(unsigned int), cadf::comms::binary::sizeOfData(data));

        cadf::comms::OutputBuffer out(sizeof(unsigned int));
        cadf::comms::binary::serializeData(data, &out);

        cadf::comms::InputBuffer in(out.getData(), out.getSize());
        BOOST_CHECK_EQUAL(data, cadf::comms::binary::deserializeData<cadf::comms::HandshakeCompleteData>(&in));
    }

    /**
     * Verify that the message can be serialized and deserialized from JSON
     */
    BOOST_AUTO_TEST_CASE(HandshakeCompleteJSONSerializationTest) {
        cadf::comms::HandshakeCompleteData data({7409});
        cadf::comms::HandshakeCompleteMessage msg(data);

        std::string expectedDataJson = "{\"version\":7409}";
        std::string expectedJson = "{\"data\":" + expectedDataJson + ",\"instance\":123,\"message\":\"HandshakeCompleteMessage\",\"type\":321}";

        cadf::comms::OutputBuffer out(expectedJson.size() + 1);
        cadf::comms::dom::Serializer<cadf::comms::HandshakeCompleteData> serializer(cadf::dom::json::JsonConverter::instance(), &msg, 321, 123);
        serializer.serialize(&out);
        BOOST_CHECK_EQUAL(expectedJson, out.getData());

        cadf::comms::InputBuffer in(out.getData(), out.getSize());
        cadf::comms::dom::Deserializer deserializer(cadf::dom::json::JsonConverter::instance(), &in);
        BOOST_CHECK_EQUAL("HandshakeCompleteMessage", deserializer.getMessageType());
        BOOST_CHECK_EQUAL(data, deserializer.getData<cadf::comms::HandshakeCompleteData>());
    }

    BOOST_AUTO_TEST_SUITE_END()
