
#define BOOST_TEST_DYN_LINK
#ifdef STAND_ALONE
#   define BOOST_TEST_MODULE Main
#endif
#include <boost/test/unit_test.hpp>

#include "comms/message/MessageRegistry.h"
#include "comms/network/serializer/local/Serializer.h"

#include "TestMessage.h"

#include <fakeit.hpp>

// Helper classes for the MessageRegistryTest
namespace MessageRegistryTest {

    /*
     * Helper fixture for initializing and preparing all of the necessary mocks
     */
    struct SetupMocks {

            SetupMocks() {
                fakeit::When(Method(mockFactory, registerMessage)).AlwaysDo([&](cadf::comms::IMessage* msg, cadf::comms::ISerializerFactory* factory) {
                    messages.push_back(msg);
                    factories.push_back(factory);
                });
            }

            ~SetupMocks() {
                verifyAllMocksChecked();
                for (auto m: messages)
                    delete(m);
                for(auto f: factories)
                    delete(f);
            }

            void verifyAllMocksChecked() {
                fakeit::VerifyNoOtherInvocations(mockFactory);
            }

            // Mocks to use for testing
            fakeit::Mock<cadf::comms::MessageFactory<cadf::comms::local::LocalProtocol>> mockFactory;

            // Record what all messages have been recorded
            std::vector<cadf::comms::IMessage*> messages;
            std::vector<cadf::comms::ISerializerFactory*> factories;
    };
}

BOOST_AUTO_TEST_SUITE(MessageRegistry_Test_Suite)

/**
 * Verify that a Registry with no messages will not register any messages
 */
BOOST_FIXTURE_TEST_CASE(RegisterNoMessage, MessageRegistryTest::SetupMocks) {
    cadf::comms::MessageRegistry<cadf::comms::local::LocalProtocol> registry;
    registry.registerMessages(&(mockFactory.get()));
    fakeit::Verify(Method(mockFactory, registerMessage)).Exactly(3);

    BOOST_CHECK_EQUAL(3, messages.size());
    BOOST_CHECK_EQUAL("HandshakeInitMessage", messages[0]->getType());
    BOOST_CHECK_EQUAL("HandshakeResponseMessageV1", messages[1]->getType());
    BOOST_CHECK_EQUAL("HandshakeCompleteMessage", messages[2]->getType());

    BOOST_CHECK_EQUAL(3, factories.size());
}

/**
 * Verify that a registry with a single message will register exactly once.
 */
BOOST_FIXTURE_TEST_CASE(RegisterOneMessage, MessageRegistryTest::SetupMocks) {
    cadf::comms::MessageRegistry<cadf::comms::local::LocalProtocol, TestMessage1> registry;
    registry.registerMessages(&(mockFactory.get()));
    fakeit::Verify(Method(mockFactory, registerMessage)).Exactly(4);

    BOOST_CHECK_EQUAL(4, messages.size());
    BOOST_CHECK_EQUAL("TestMessage1", messages[0]->getType());
    BOOST_CHECK_EQUAL("HandshakeInitMessage", messages[1]->getType());
    BOOST_CHECK_EQUAL("HandshakeResponseMessageV1", messages[2]->getType());
    BOOST_CHECK_EQUAL("HandshakeCompleteMessage", messages[3]->getType());

    BOOST_CHECK_EQUAL(4, factories.size());
}

/**
 * Verify that a registry with two messages will register exactly twice.
 */
BOOST_FIXTURE_TEST_CASE(RegisterTwoMessages, MessageRegistryTest::SetupMocks) {
    cadf::comms::MessageRegistry<cadf::comms::local::LocalProtocol, TestMessage1, TestMessage2> registry;
    registry.registerMessages(&(mockFactory.get()));
    fakeit::Verify(Method(mockFactory, registerMessage)).Exactly(5);

    BOOST_CHECK_EQUAL(5, messages.size());
    BOOST_CHECK_EQUAL("TestMessage1", messages[0]->getType());
    BOOST_CHECK_EQUAL("TestMessage2", messages[1]->getType());
    BOOST_CHECK_EQUAL("HandshakeInitMessage", messages[2]->getType());
    BOOST_CHECK_EQUAL("HandshakeResponseMessageV1", messages[3]->getType());
    BOOST_CHECK_EQUAL("HandshakeCompleteMessage", messages[4]->getType());

    BOOST_CHECK_EQUAL(5, factories.size());
}

/**
 * Verify that a registry with three messages will register exactly three times.
 */
BOOST_FIXTURE_TEST_CASE(RegisterThreeMessages, MessageRegistryTest::SetupMocks) {
    cadf::comms::MessageRegistry<cadf::comms::local::LocalProtocol, TestMessage1, TestMessage2, TestMessage3> registry;
    registry.registerMessages(&(mockFactory.get()));
    fakeit::Verify(Method(mockFactory, registerMessage)).Exactly(6);

    BOOST_CHECK_EQUAL(6, messages.size());
    BOOST_CHECK_EQUAL("TestMessage1", messages[0]->getType());
    BOOST_CHECK_EQUAL("TestMessage2", messages[1]->getType());
    BOOST_CHECK_EQUAL("TestMessage3", messages[2]->getType());
    BOOST_CHECK_EQUAL("HandshakeInitMessage", messages[3]->getType());
    BOOST_CHECK_EQUAL("HandshakeResponseMessageV1", messages[4]->getType());
    BOOST_CHECK_EQUAL("HandshakeCompleteMessage", messages[5]->getType());

    BOOST_CHECK_EQUAL(6, factories.size());
}

BOOST_AUTO_TEST_SUITE_END()
