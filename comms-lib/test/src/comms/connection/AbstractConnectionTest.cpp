
#define BOOST_TEST_DYN_LINK
#ifdef STAND_ALONE
#   define BOOST_TEST_MODULE Main
#endif
#include <boost/test/unit_test.hpp>

#include "comms/connection/Connection.h"

#include <TestMessage.h>
#include <fakeit.hpp>

// Helper classes for the AbstractConnectionTest
namespace AbstractConnectionTest {

    /**
     * A bare bones implementation of the AbstractConnection, to ensure that the functionality of the
     * AbstractConnection can be tested in isolation.
     */
    class ConcreteAbstractConnection: public cadf::comms::AbstractConnection {

        public:
            ConcreteAbstractConnection(int type, int instance, cadf::comms::IMessageFactory *factory) : AbstractConnection(type, instance, factory) {
                BOOST_CHECK_EQUAL(type, getType());
                BOOST_CHECK_EQUAL(instance, getInstance());
            }

            // Expose the protected method so that the test can reach it
            virtual void notifyMessageRecieved(const cadf::comms::MessagePacket *packet) {
                cadf::comms::AbstractConnection::notifyMessageRecieved(packet);
            }

        private:
            virtual bool isConnected() {
                return false;
            }
            virtual bool connect() {
                return false;
            }
            virtual bool disconnect() {
                return false;
            }
            virtual bool sendMessage(const cadf::comms::IMessage *msg, int recipient, int instance) {
                return false;
            }

            virtual bool sendPacket(const cadf::comms::MessagePacket *packet) {
                return false;
            }
    };

    /*
     * Helper fixture for initializing and preparing all of the necessary mocks
     */
    struct SetupMocks {

            SetupMocks() {
                fakeit::When(Method(mockListener, messageReceived)).AlwaysReturn();
            }

            ~SetupMocks() {
                verifyAllMocksChecked();
            }

            void verifyAllMocksChecked() {
                fakeit::VerifyNoOtherInvocations(mockFactory, mockPacket, mockListener);
            }

            // Mocks to use for testing
            fakeit::Mock<cadf::comms::IMessageFactory> mockFactory;
            fakeit::Mock<cadf::comms::MessagePacket> mockPacket;
            fakeit::Mock<cadf::comms::IMessageListener> mockListener;
    };

    /**
     * Helper fixture to initialize the LocalConnection instance that is to be used for the tests
     */
    struct TestFixture: public SetupMocks {

            TestFixture() : SetupMocks(), conn(123, 789, &mockFactory.get()) {
                BOOST_CHECK_EQUAL(123, conn.getType());
                BOOST_CHECK_EQUAL(789, conn.getInstance());

                // Verify the initialization of the class to be tested
                verifyAllMocksChecked();
            }

            // The class to test
            ConcreteAbstractConnection conn;
    };

}

/**
 * Unit test for the LocalConnection
 */
BOOST_AUTO_TEST_SUITE(AbstractConnection_Test_Suite)

/**
 * Verify receiving a message without a registered listener does nothing
 */
    BOOST_FIXTURE_TEST_CASE(ReceiveMessageWithoutListenerTest, AbstractConnectionTest::TestFixture) {
        conn.notifyMessageRecieved(&mockPacket.get());
        verifyAllMocksChecked();
    }

/**
 * Verify receiving a message without a registered listener does nothing
 */
BOOST_FIXTURE_TEST_CASE(ReceiveMessageWithListenerRegisteredTest, AbstractConnectionTest::TestFixture) {
    conn.addMessageListener(&mockListener.get());
    verifyAllMocksChecked();

    // A message received is not notified to the listener
    conn.notifyMessageRecieved(&mockPacket.get());
    fakeit::Verify(Method(mockListener, messageReceived).Using(&mockPacket.get())).Once();
    verifyAllMocksChecked();

    // Make sure that it can happen several more times
    conn.notifyMessageRecieved(&mockPacket.get());
    fakeit::Verify(Method(mockListener, messageReceived).Using(&mockPacket.get())).Exactly(2);
    conn.notifyMessageRecieved(&mockPacket.get());
    fakeit::Verify(Method(mockListener, messageReceived).Using(&mockPacket.get())).Exactly(3);
    verifyAllMocksChecked();
}

/**
 * Verify removing a listener stops it from receiving messages
 */
BOOST_FIXTURE_TEST_CASE(ReceiveMessageWithListenerRegisteredThenRemovedTest, AbstractConnectionTest::TestFixture) {
    conn.addMessageListener(&mockListener.get());
    verifyAllMocksChecked();

    // A message received is not notified to the listener
    conn.notifyMessageRecieved(&mockPacket.get());
    fakeit::Verify(Method(mockListener, messageReceived).Using(&mockPacket.get())).Once();
    verifyAllMocksChecked();

    // Remove the listener
    conn.removeMessageListener(&mockListener.get());
    verifyAllMocksChecked();

    // Make sure that subsequent messages are no longer sent to the listener
    conn.notifyMessageRecieved(&mockPacket.get());
    conn.notifyMessageRecieved(&mockPacket.get());
    conn.notifyMessageRecieved(&mockPacket.get());
    verifyAllMocksChecked();
}

    BOOST_AUTO_TEST_SUITE_END()
