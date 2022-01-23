#define BOOST_TEST_DYN_LINK
#ifdef STAND_ALONE
#   define BOOST_TEST_MODULE Main
#endif
#include <boost/test/unit_test.hpp>
#include <fakeit.hpp>

#include "comms/network/server/BasicServerConnection.h"
#include "comms/network/serializer/local/Serializer.h"

namespace BasicServerConnectionTest {

    /*
     * Helper fixture for initializing and preparing all of the necessary mocks
     */
    struct SetupMocks {

            SetupMocks() {
                fakeit::Fake(Method(mockSocket, addListener));
                fakeit::Fake(Method(mockSocket, removeListener));
                fakeit::Fake(Method(mockListener, messageReceived));
                fakeit::When(Method(mockFactory, deserializeMessage)).AlwaysReturn(new cadf::comms::MessagePacket(&mockReceivedMessage.get(), 1, 2));
            }

            ~SetupMocks() {
                verifyAllMocksChecked();
            }

            void verifyAllMocksChecked() {
                fakeit::VerifyNoOtherInvocations(mockSocket, mockFactory, mockListener, mockSentMessage, mockReceivedMessage, mockInBuffer);
            }

            // Mocks to use for testing
            fakeit::Mock<cadf::comms::ISocketDataHandler> mockSocket;
            fakeit::Mock<cadf::comms::MessageFactory<cadf::comms::local::LocalProtocol>> mockFactory;
            fakeit::Mock<cadf::comms::IMessageListener> mockListener;
            fakeit::Mock<cadf::comms::IMessage> mockSentMessage;
            fakeit::Mock<cadf::comms::IMessage> mockReceivedMessage;
            fakeit::Mock<cadf::comms::InputBuffer> mockInBuffer;
    };

    /**
     * Helper fixture to initialize the BasicServerConnection instance that is to be used for the tests
     */
    struct TestFixture: public SetupMocks {

            TestFixture() : SetupMocks() {
                cadf::comms::BasicServerConnectionFactory<cadf::comms::local::LocalProtocol> factory(&mockFactory.get());
                conn = static_cast<cadf::comms::BasicServerConnection<cadf::comms::local::LocalProtocol>*>(factory.createConnection(2, 3, &mockSocket.get()));
                fakeit::Verify(Method(mockSocket, addListener).Using(conn)).Once();

                verifyAllMocksChecked();
            }

            ~TestFixture() {
                delete (conn);
                fakeit::Verify(Method(mockSocket, removeListener).Using(conn)).Once();
                verifyAllMocksChecked();
            }

            /** The connection to test */
            cadf::comms::BasicServerConnection<cadf::comms::local::LocalProtocol> *conn;
    };

}

BOOST_AUTO_TEST_SUITE(BasicServerConnection_Test_Suite)

/**
 * Verify that methods that are independent of the state work as expected
 */
    BOOST_FIXTURE_TEST_CASE(VerifyUnchangingValuesTest, BasicServerConnectionTest::TestFixture) {
        BOOST_CHECK(conn->isConnected());
        BOOST_CHECK(conn->connect());
        BOOST_CHECK(!conn->disconnect());
        BOOST_CHECK_EQUAL(2, conn->getType());
        BOOST_CHECK_EQUAL(3, conn->getInstance());
    }

    /**
     * Verify that the process of sending a message works
     */
    BOOST_FIXTURE_TEST_CASE(VerifySendMessageSuccessfulTest, BasicServerConnectionTest::TestFixture) {
        cadf::comms::OutputBuffer *out = new cadf::comms::OutputBuffer(1);
        fakeit::When(Method(mockSocket, send)).AlwaysReturn(true);
        fakeit::When(Method(mockFactory, serializeMessage)).AlwaysDo([&](const cadf::comms::MessagePacket &packet) {
            BOOST_CHECK_EQUAL(123, packet.getRecipientType());
            BOOST_CHECK_EQUAL(321, packet.getRecipientInstance());
            BOOST_CHECK_EQUAL(&mockSentMessage.get(), packet.getMessage());
            return out;
        });

        BOOST_CHECK(conn->sendMessage(&mockSentMessage.get(), 123, 321));
        fakeit::Verify(Method(mockFactory, serializeMessage)).Once();
        fakeit::Verify(Method(mockSocket, send).Using(out)).Once();
    }

    /**
     * Verify that the process of sending a message works
     */
    BOOST_FIXTURE_TEST_CASE(VerifySendMessageUnsuccessfulTest, BasicServerConnectionTest::TestFixture) {
        cadf::comms::OutputBuffer *out = new cadf::comms::OutputBuffer(1);
        fakeit::When(Method(mockSocket, send)).AlwaysReturn(false);
        fakeit::When(Method(mockFactory, serializeMessage)).AlwaysDo([&](const cadf::comms::MessagePacket &packet) {
            BOOST_CHECK_EQUAL(59, packet.getRecipientType());
            BOOST_CHECK_EQUAL(251, packet.getRecipientInstance());
            BOOST_CHECK_EQUAL(&mockSentMessage.get(), packet.getMessage());
            return out;
        });

        BOOST_CHECK(!conn->sendMessage(&mockSentMessage.get(), 59, 251));
        fakeit::Verify(Method(mockFactory, serializeMessage)).Once();
        fakeit::Verify(Method(mockSocket, send).Using(out)).Once();
    }

    /**
     * Verify that the process of sending a Packet works
     */
    BOOST_FIXTURE_TEST_CASE(VerifySendPacketSuccessfulTest, BasicServerConnectionTest::TestFixture) {
        cadf::comms::OutputBuffer *out = new cadf::comms::OutputBuffer(1);
        fakeit::When(Method(mockSocket, send)).AlwaysReturn(true);
        fakeit::When(Method(mockFactory, serializeMessage)).AlwaysDo([&](const cadf::comms::MessagePacket &packet) {
            BOOST_CHECK_EQUAL(123, packet.getRecipientType());
            BOOST_CHECK_EQUAL(321, packet.getRecipientInstance());
            BOOST_CHECK_EQUAL(&mockSentMessage.get(), packet.getMessage());
            return out;
        });

        cadf::comms::MessagePacket toSend(&mockSentMessage.get(), 123, 321);
        BOOST_CHECK(conn->sendPacket(&toSend));
        fakeit::Verify(Method(mockFactory, serializeMessage)).Once();
        fakeit::Verify(Method(mockSocket, send).Using(out)).Once();
    }

    /**
     * Verify that the process of sending a Packet works
     */
    BOOST_FIXTURE_TEST_CASE(VerifySendPacketUnsuccessfulTest, BasicServerConnectionTest::TestFixture) {
        cadf::comms::OutputBuffer *out = new cadf::comms::OutputBuffer(1);
        fakeit::When(Method(mockSocket, send)).AlwaysReturn(false);
        fakeit::When(Method(mockFactory, serializeMessage)).AlwaysDo([&](const cadf::comms::MessagePacket &packet) {
            BOOST_CHECK_EQUAL(897, packet.getRecipientType());
            BOOST_CHECK_EQUAL(648, packet.getRecipientInstance());
            BOOST_CHECK_EQUAL(&mockSentMessage.get(), packet.getMessage());
            return out;
        });

        cadf::comms::MessagePacket toSend(&mockSentMessage.get(), 897, 648);
        BOOST_CHECK(!conn->sendPacket(&toSend));
        fakeit::Verify(Method(mockFactory, serializeMessage)).Once();
        fakeit::Verify(Method(mockSocket, send).Using(out)).Once();
    }

    /**
     * Verify that the process of receiving a Message works
     */
    BOOST_FIXTURE_TEST_CASE(VerifyReceiveMessageNoListenerTest, BasicServerConnectionTest::TestFixture) {
        fakeit::When(Method(mockFactory, deserializeMessage)).AlwaysReturn(new cadf::comms::MessagePacket(&mockReceivedMessage.get(), 8, 3));

        conn->messageReceived(&mockInBuffer.get());
        fakeit::Verify(Method(mockFactory, deserializeMessage).Using(&mockInBuffer.get())).Once();
    }

    /**
     * Verify that the process of receiving a Message works
     */
    BOOST_FIXTURE_TEST_CASE(VerifyReceiveMessageWithListenerTest, BasicServerConnectionTest::TestFixture) {

        // Add the listener and it will receive the message
        cadf::comms::MessagePacket *expectedPacket = new cadf::comms::MessagePacket(&mockReceivedMessage.get(), 8, 3);
        fakeit::When(Method(mockFactory, deserializeMessage)).AlwaysReturn(expectedPacket);

        conn->addMessageListener(&mockListener.get());
        conn->messageReceived(&mockInBuffer.get());
        fakeit::Verify(Method(mockFactory, deserializeMessage).Using(&mockInBuffer.get())).Once();
        fakeit::Verify(Method(mockListener, messageReceived).Using(expectedPacket)).Once();

        // Remove the listener and it stops receiving messages
        expectedPacket = new cadf::comms::MessagePacket(&mockReceivedMessage.get(), 8, 3);
        fakeit::When(Method(mockFactory, deserializeMessage)).AlwaysReturn(expectedPacket);

        conn->removeMessageListener(&mockListener.get());
        conn->messageReceived(&mockInBuffer.get());
        fakeit::Verify(Method(mockFactory, deserializeMessage).Using(&mockInBuffer.get())).Twice();
    }

    BOOST_AUTO_TEST_SUITE_END()
