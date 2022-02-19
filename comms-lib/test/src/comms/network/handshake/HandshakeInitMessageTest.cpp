
#define BOOST_TEST_DYN_LINK
#ifdef STAND_ALONE
#   define BOOST_TEST_MODULE Main
#endif
#include <boost/test/unit_test.hpp>
#include <sstream>

#include "comms/network/handshake/HandshakeInitMessage.h"
#include "comms/network/serializer/binary/Serializer.h"
#include "comms/network/serializer/dom/Serializer.h"

/**
 * Ensure the correct functionality of the HandshakeInitMessage
 */
BOOST_AUTO_TEST_SUITE(HandshakeInitMessage_Test_Suite)

/**
 * Verify that the data can be properly compared
 */
    BOOST_AUTO_TEST_CASE(HandshakeInitDataEqualTest) {
        BOOST_CHECK_EQUAL(cadf::comms::HandshakeInitData( { 1 }), cadf::comms::HandshakeInitData( { 1 }));
        BOOST_CHECK_EQUAL(cadf::comms::HandshakeInitData( { 456 }), cadf::comms::HandshakeInitData( { 456 }));
        BOOST_CHECK_EQUAL(cadf::comms::HandshakeInitData( { 1934 }), cadf::comms::HandshakeInitData( { 1934 }));

        BOOST_CHECK_EQUAL(false, cadf::comms::HandshakeInitData( { 1 }) == cadf::comms::HandshakeInitData( { 2 }));
        BOOST_CHECK_EQUAL(false, cadf::comms::HandshakeInitData( { 123 }) == cadf::comms::HandshakeInitData( { 234 }));
        BOOST_CHECK_EQUAL(false, cadf::comms::HandshakeInitData( { 1934 }) == cadf::comms::HandshakeInitData( { 4391 }));
    }

    /**
     * Verify that the data can be properly streamed
     */
    BOOST_AUTO_TEST_CASE(HandshakeInitDataStreamTest) {
        std::stringstream ss;
        ss << cadf::comms::HandshakeInitData( { 341 });
        BOOST_CHECK_EQUAL("[ maxVersion = 341 ]", ss.str());

        ss.str("");
        ss << cadf::comms::HandshakeInitData( { 5 });
        BOOST_CHECK_EQUAL("[ maxVersion = 5 ]", ss.str());

        ss.str("");
        ss << cadf::comms::HandshakeInitData( { 89 });
        BOOST_CHECK_EQUAL("[ maxVersion = 89 ]", ss.str());
    }

    /**
     * Verify that the message can be initialized
     */
    BOOST_AUTO_TEST_CASE(HandshakeInitMessageInitTest) {
        cadf::comms::HandshakeInitData data;
        BOOST_CHECK_EQUAL(1, data.maxVersion);

        cadf::comms::HandshakeInitMessage defaultMsg;
        BOOST_CHECK_EQUAL(1, defaultMsg.getData().maxVersion);
        BOOST_CHECK_EQUAL("HandshakeInitMessage", defaultMsg.getType());
    }

    /**
     * Verify that the message can be copied
     */
    BOOST_AUTO_TEST_CASE(HandshakeInitMessageCopyTest) {
        cadf::comms::HandshakeInitMessage orig;
        cadf::comms::AbstractDataMessage<cadf::comms::HandshakeInitData> *copy = orig.newInstance();
        BOOST_CHECK(&orig != copy);
        BOOST_CHECK_EQUAL(orig.getType(), copy->getType());
        BOOST_CHECK_EQUAL(orig.getData().maxVersion, copy->getData().maxVersion);
        delete(copy);
    }

    /**
     * Verify that the message can be serialized and deserialized from binary
     */
    BOOST_AUTO_TEST_CASE(HandshakeInitBinarySerializationTest) {
        cadf::comms::HandshakeInitData data({837});
        BOOST_CHECK_EQUAL(sizeof(unsigned int), cadf::comms::binary::sizeOfData(data));

        cadf::comms::OutputBuffer out(sizeof(unsigned int));
        cadf::comms::binary::serializeData(data, &out);

        cadf::comms::InputBuffer in(out.getData(), out.getSize());
        BOOST_CHECK_EQUAL(data, cadf::comms::binary::deserializeData<cadf::comms::HandshakeInitData>(&in));
    }

    /**
     * Verify that the message can be serialized and deserialized from JSON
     */
    BOOST_AUTO_TEST_CASE(HandshakeCompleteJSONSerializationTest) {
        cadf::comms::HandshakeInitData data;
        cadf::comms::HandshakeInitMessage msg;

        std::string expectedDataJson = "{\"maxVersion\":1}";
        std::string expectedJson = "{\"data\":" + expectedDataJson + ",\"instance\":345,\"message\":\"HandshakeInitMessage\",\"type\":234}";

        cadf::comms::OutputBuffer out(expectedJson.size() + 1);
        cadf::comms::dom::Serializer<cadf::comms::HandshakeInitData> serializer(cadf::dom::json::JsonConverter::instance(), &msg, 234, 345);
        serializer.serialize(&out);
        BOOST_CHECK_EQUAL(expectedJson, out.getData());

        cadf::comms::InputBuffer in(out.getData(), out.getSize());
        cadf::comms::dom::Deserializer deserializer(cadf::dom::json::JsonConverter::instance(), &in);
        BOOST_CHECK_EQUAL("HandshakeInitMessage", deserializer.getMessageType());
        BOOST_CHECK_EQUAL(data, deserializer.getData<cadf::comms::HandshakeInitData>());
    }

    BOOST_AUTO_TEST_SUITE_END()
