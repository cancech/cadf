
#define BOOST_TEST_DYN_LINK
#ifdef STAND_ALONE
#   define BOOST_TEST_MODULE Main
#endif
#include <boost/test/unit_test.hpp>
#include <fakeit.hpp>

#include "comms/connection/LocalConnection.h"
#include "comms/connection/ConnectionException.h"

#include "TestMessage.h"

// Helper classes for the LocalConnectionTest
namespace LocalConnectionTest {
    /*
     * Helper fixture for initializing and preparing all of the necessary mocks
     */
    struct SetupMocks {

            SetupMocks() {
                fakeit::When(Method(mockMessage, getType)).AlwaysReturn("MessageType");
                fakeit::When(Method(mockFactory, isMessageRegistered).Using("MessageType")).AlwaysReturn(false);

                fakeit::When(Method(mockBus, connected)).AlwaysReturn();
                fakeit::When(Method(mockBus, disconnected)).AlwaysReturn();
                fakeit::When(Method(mockBus, sendMessage)).AlwaysReturn();

                fakeit::When(Method(mockListener, messageReceived)).AlwaysReturn();
            }

            ~SetupMocks() {
                verifyAllMocksChecked();
            }

            void verifyAllMocksChecked() {
                fakeit::VerifyNoOtherInvocations(mockFactory, mockMessage, mockBus, mockBusConnection, mockListener);
            }

            // Mocks to use for testing
            fakeit::Mock<cadf::comms::IMessageFactory> mockFactory;
            fakeit::Mock<cadf::comms::IMessage> mockMessage;
            fakeit::Mock<cadf::comms::IBus> mockBus;
            fakeit::Mock<cadf::comms::IBusConnection> mockBusConnection;
            fakeit::Mock<cadf::comms::IMessageListener> mockListener;
    };

    /**
     * Helper fixture to initialize the LocalConnection instance that is to be used for the tests
     */
    struct TestFixture: public SetupMocks {

            TestFixture() : SetupMocks(), conn(321, 654, &mockFactory.get()) {
                BOOST_CHECK_EQUAL(321, conn.getType());
                BOOST_CHECK_EQUAL(654, conn.getInstance());
                // Verify the initialization of the class to be tested
                verifyAllMocksChecked();
            }

            void registerBus() {
                conn.registerBus(&mockBus.get());
                verifyAllMocksChecked();
            }

            // The class to test
            cadf::comms::LocalConnection conn;
    };

}

/**
 * Unit test for the LocalConnection
 */
BOOST_AUTO_TEST_SUITE(LocalConnection_Test_Suite)

/**
 * Verify that can create a LocalConnection, and nothing happens without a registered bus.
 */
    BOOST_FIXTURE_TEST_CASE(LocalConnectionNoBusRegisteredTest, LocalConnectionTest::TestFixture) {
        // Verify that it cannot do anything without a bus
        BOOST_CHECK_EQUAL(false, conn.isConnected());
        BOOST_REQUIRE_THROW(conn.connect(), cadf::comms::BusException);
        BOOST_REQUIRE_THROW(conn.disconnect(), cadf::comms::BusException);
        BOOST_REQUIRE_THROW(conn.sendMessage(&mockMessage.get()), cadf::comms::BusException);
        verifyAllMocksChecked();
    }

    /**
     * Verify that can create a LocalConnection, and with a bus registered can connect and disconnect
     */
    BOOST_FIXTURE_TEST_CASE(LocalConnectionLifecycleTest, LocalConnectionTest::TestFixture) {
        // Register the bus
        BOOST_CHECK_EQUAL(false, conn.isConnected());
        registerBus();
        BOOST_CHECK_EQUAL(false, conn.isConnected());
        verifyAllMocksChecked();

        // Connect
        BOOST_CHECK(conn.connect());
        fakeit::Verify(Method(mockBus, connected).Using((cadf::comms::IBusConnection*)&conn)).Once();
        BOOST_CHECK_EQUAL(true, conn.isConnected());
        verifyAllMocksChecked();

        // Disconnect
        BOOST_CHECK(conn.disconnect());
        fakeit::Verify(Method(mockBus, disconnected).Using((cadf::comms::IBusConnection*)&conn)).Once();
        BOOST_CHECK_EQUAL(false, conn.isConnected());
        verifyAllMocksChecked();
    }

    /**
     * Verify that connecting when connected does nothing
     */
    BOOST_FIXTURE_TEST_CASE(LocalConnectionMultipleConnectsTest, LocalConnectionTest::TestFixture) {
        registerBus();

        // First connection performs the action
        BOOST_CHECK_EQUAL(false, conn.isConnected());
        BOOST_CHECK(conn.connect());
        fakeit::Verify(Method(mockBus, connected).Using((cadf::comms::IBusConnection*)&conn)).Once();
        BOOST_CHECK_EQUAL(true, conn.isConnected());
        verifyAllMocksChecked();

        // Subsequent connections do nothing
        BOOST_CHECK(conn.connect());
        BOOST_CHECK_EQUAL(true, conn.isConnected());
        BOOST_CHECK(conn.connect());
        BOOST_CHECK_EQUAL(true, conn.isConnected());
        BOOST_CHECK(conn.connect());
        BOOST_CHECK_EQUAL(true, conn.isConnected());
        verifyAllMocksChecked();
    }

    /**
     * Verify that disconnecting when disconnected does nothing
     */
    BOOST_FIXTURE_TEST_CASE(LocalConnectionDisconnectWhenDisconnectedTest, LocalConnectionTest::TestFixture) {
        registerBus();

        BOOST_CHECK_EQUAL(false, conn.isConnected());
        BOOST_CHECK(conn.disconnect());
        BOOST_CHECK_EQUAL(false, conn.isConnected());
        BOOST_CHECK(conn.disconnect());
        BOOST_CHECK_EQUAL(false, conn.isConnected());
        BOOST_CHECK(conn.disconnect());
        verifyAllMocksChecked();
    }

    /**
     * Verify that sending a message when not connected does nothing.
     */
    BOOST_FIXTURE_TEST_CASE(LocalConnectionSendMessageWhenNotConnectedTest, LocalConnectionTest::TestFixture) {
        registerBus();

        BOOST_CHECK_EQUAL(false, conn.isConnected());
        BOOST_REQUIRE_THROW(conn.sendMessage(&mockMessage.get()), cadf::comms::MessageSendingException);
        BOOST_REQUIRE_THROW(conn.sendMessage(&mockMessage.get()), cadf::comms::MessageSendingException);
        BOOST_REQUIRE_THROW(conn.sendMessage(&mockMessage.get()), cadf::comms::MessageSendingException);
        fakeit::Verify(Method(mockMessage, getType)).Exactly(3);
        verifyAllMocksChecked();
    }

    /**
     * Verify a message cannot be sent when connected, if it isn't registered with the factory.
     */
    BOOST_FIXTURE_TEST_CASE(LocalConnectionSendMessageWhenConnectedMessageNotRegisteredTest, LocalConnectionTest::TestFixture) {
        registerBus();
        BOOST_CHECK_EQUAL(false, conn.isConnected());
        BOOST_CHECK(conn.connect());
        fakeit::Verify(Method(mockBus, connected).Using((cadf::comms::IBusConnection*)&conn)).Once();
        BOOST_CHECK_EQUAL(true, conn.isConnected());

        BOOST_REQUIRE_THROW(conn.sendMessage(&mockMessage.get()), cadf::comms::MessageSendingException);
        fakeit::Verify(Method(mockMessage, getType)).Exactly(2);
        fakeit::Verify(Method(mockFactory, isMessageRegistered).Using("MessageType")).Once();
        verifyAllMocksChecked();
    }

    /**
     * Verify a message can be sent when connected and when the message is registered with the factory.
     */
    BOOST_FIXTURE_TEST_CASE(LocalConnectionSendMessageWhenConnectedMessageRegisteredTest, LocalConnectionTest::TestFixture) {
        registerBus();
        BOOST_CHECK_EQUAL(false, conn.isConnected());
        BOOST_CHECK(conn.connect());
        fakeit::Verify(Method(mockBus, connected).Using((cadf::comms::IBusConnection*)&conn)).Once();
        BOOST_CHECK_EQUAL(true, conn.isConnected());

        fakeit::When(Method(mockFactory, isMessageRegistered)).AlwaysReturn(true);
        fakeit::When(Method(mockBus, sendMessage)).Do([this](cadf::comms::IBusConnection* passedConn, const cadf::comms::MessagePacket* packet) {
            BOOST_CHECK_EQUAL(&conn, passedConn);
            BOOST_CHECK_EQUAL(&mockMessage.get(), packet->getMessage());
        });
        BOOST_REQUIRE_NO_THROW(conn.sendMessage(&mockMessage.get()));
        fakeit::Verify(Method(mockMessage, getType)).Once();
        fakeit::Verify(Method(mockFactory, isMessageRegistered).Using("MessageType")).Once();
        fakeit::Verify(Method(mockBus, sendMessage)).Once();
        verifyAllMocksChecked();
    }

    /**
     * Verify that can create a LocalConnection, and a message that is not registered with the factory is not propagated
     */
    BOOST_FIXTURE_TEST_CASE(LocalConnectionReceiveNotRegisteredMessageTest, LocalConnectionTest::TestFixture) {
        conn.addMessageListener(&mockListener.get());
        verifyAllMocksChecked();

        cadf::comms::MessagePacket packet(&mockMessage.get(), cadf::comms::ConnectionConstants::BROADCAST, cadf::comms::ConnectionConstants::BROADCAST);
        conn.sendMessage(&mockBusConnection.get(), &packet);
        fakeit::Verify(Method(mockMessage, getType)).Once();
        fakeit::Verify(Method(mockFactory, isMessageRegistered)).Once();
        verifyAllMocksChecked();
    }

    /**
     * Verify that can create a LocalConnection, and a message registered with the factory is propagated
     */
    BOOST_FIXTURE_TEST_CASE(LocalConnectionReceiveRegisteredMessageTest, LocalConnectionTest::TestFixture) {
        conn.addMessageListener(&mockListener.get());
        verifyAllMocksChecked();

        fakeit::When(Method(mockFactory, isMessageRegistered).Using("MessageType")).AlwaysReturn(true);
        cadf::comms::MessagePacket packet(&mockMessage.get(), cadf::comms::ConnectionConstants::BROADCAST, cadf::comms::ConnectionConstants::BROADCAST);
        conn.sendMessage(&mockBusConnection.get(), &packet);
        fakeit::Verify(Method(mockMessage, getType)).Once();
        fakeit::Verify(Method(mockFactory, isMessageRegistered)).Once();
        fakeit::Verify(Method(mockListener, messageReceived).Using(&packet)).Once();
        verifyAllMocksChecked();
    }

    BOOST_AUTO_TEST_SUITE_END()
