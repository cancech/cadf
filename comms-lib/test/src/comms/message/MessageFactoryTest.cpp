#define BOOST_TEST_DYN_LINK
#ifdef STAND_ALONE
#   define BOOST_TEST_MODULE Main
#endif
#include <boost/test/unit_test.hpp>

#include "comms/message/MessageFactory.h"
#include "comms/message/MessageException.h"
#include "comms/message/MessagePacket.h"
#include "comms/network/serializer/Serializer.h"

#include "TestMessage.h"
#include "TestProtocol.h"

BOOST_AUTO_TEST_SUITE(MessageFactory_Test_Suite)

/**
 * Verify that can create a registered message
 */
    BOOST_AUTO_TEST_CASE(RegisterAndCreateMessage) {
        cadf::comms::MessageFactory<MockProtocol> factory;
        BOOST_CHECK_EQUAL(false, factory.isMessageRegistered("TestMessage11"));
        factory.registerMessage(new TestMessage1(), new MockSerializerFactory());
        BOOST_CHECK(factory.isMessageRegistered("TestMessage1"));

        auto msg = factory.createMessage("TestMessage1");
        BOOST_CHECK_EQUAL("TestMessage1", msg->getType());
        delete (msg);
    }

    /**
     * Verify that an exception is thrown when trying to create a message that has not yet been registered.
     */
    BOOST_AUTO_TEST_CASE(CreateWhenNotRegistered) {
        cadf::comms::MessageFactory<MockProtocol> factory;
        BOOST_CHECK_EQUAL(false, factory.isMessageRegistered("TestMessage1"));
        BOOST_REQUIRE_THROW(factory.createMessage("TestMessage1"), cadf::comms::InvalidMessageTypeException);
    }

    /**
     * Verify that an exception is thrown when trying to register the same message a second time.
     */
    BOOST_AUTO_TEST_CASE(RegisterTwice) {
        TestMessage1 *msg = new TestMessage1();
        MockSerializerFactory *serializer1 = new MockSerializerFactory();
        TestMessage1 *msg2 = new TestMessage1();
        MockSerializerFactory *serializer2 = new MockSerializerFactory();

        cadf::comms::MessageFactory<MockProtocol> factory;
        BOOST_CHECK_EQUAL(false, factory.isMessageRegistered("TestMessage1"));
        factory.registerMessage(msg, serializer1);
        BOOST_CHECK(factory.isMessageRegistered("TestMessage1"));

        BOOST_REQUIRE_THROW(factory.registerMessage(msg2, serializer2), cadf::comms::InvalidMessageTypeException);
        BOOST_CHECK(factory.isMessageRegistered("TestMessage1"));
        // msg is cleaned up by the factory
        delete (msg2);
        delete (serializer2);
    }

    /**
     * Verify that a message is serialized and deserialized as expected.
     *
     * Note the serialization and deserialization is faked, so only checking that the expected "fake" result is produced.
     */
    BOOST_AUTO_TEST_CASE(Serialization) {
        cadf::comms::MessageFactory<MockProtocol> factory;
        BOOST_CHECK_EQUAL(false, factory.isMessageRegistered("TestMessage1"));
        factory.registerMessage(new TestMessage1(), new MockSerializerFactory());
        BOOST_CHECK(factory.isMessageRegistered("TestMessage1"));

        TestMessage1 msg;
        cadf::comms::MessagePacket packet(&msg, 1, 2);
        cadf::comms::OutputBuffer *buffer = factory.serializeMessage(packet);
        BOOST_CHECK_EQUAL(11, buffer->getSize());
        BOOST_CHECK_EQUAL("SERIALIZED", buffer->getData());
        delete (buffer);

        MockProtocol::m_expectedDeserializedMessageType = "TestMessage1";
        cadf::comms::InputBuffer in("", 0);
        cadf::comms::MessagePacket *receivedPacket = factory.deserializeMessage(&in);
        BOOST_CHECK_EQUAL("TestMessage1", receivedPacket->getMessage()->getType());
        delete (receivedPacket);
    }

    BOOST_AUTO_TEST_SUITE_END()
