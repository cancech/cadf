
#define BOOST_TEST_DYN_LINK
#ifdef STAND_ALONE
#   define BOOST_TEST_MODULE Main
#endif
#include <boost/test/unit_test.hpp>

#include "comms/message/MessageRegistry.h"
#include "comms/network/serializer/local/Serializer.h"

#include "TestMessage.h"

namespace MessageRegistryTest {

}

BOOST_AUTO_TEST_SUITE(MessageRegistryIT_Test_Suite)

/**
 * Verify that we can register a series of messages and then create them with the factory.
 */
BOOST_AUTO_TEST_CASE(RegisterAndCreateMessages) {
    // Register the messages
    cadf::comms::MessageFactory<cadf::comms::local::LocalProtocol> factory(512);
    cadf::comms::MessageRegistry<cadf::comms::local::LocalProtocol, TestMessage1, TestMessage3> registry;
    registry.registerMessages(&factory);

    // Make sure that can create the registered messages
    // TestMessage1 was registered and can be created
    auto msg = factory.createMessage("TestMessage1");
    BOOST_CHECK_EQUAL("TestMessage1", msg->getType());
    delete(msg);

    // TestMessage2 was not registered, so exception when trying to create
    BOOST_REQUIRE_THROW(factory.createMessage("TestMessage2"), cadf::comms::InvalidMessageTypeException);

    // TestMessage3 was registered and can be created
    auto msg3 = factory.createMessage("TestMessage3");
    BOOST_CHECK_EQUAL("TestMessage3", msg3->getType());
    delete(msg3);
}

BOOST_AUTO_TEST_SUITE_END()
