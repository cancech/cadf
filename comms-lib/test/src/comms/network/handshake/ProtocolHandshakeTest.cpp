#define BOOST_TEST_DYN_LINK
#ifdef STAND_ALONE
#   define BOOST_TEST_MODULE Main
#endif
#include <boost/test/unit_test.hpp>
#include <fakeit.hpp>

#include "TestProtocol.h"

#include "comms/network/handshake/ProtocolHandshake.h"

namespace ProtocolHandshakeTest {

    /*
     * Helper fixture for initializing and preparing all of the necessary mocks
     */
    struct SetupMocks {

            SetupMocks() : m_sentMsgType("") {
                fakeit::When(Method(mockSocket, addListener)).AlwaysReturn();
                fakeit::When(Method(mockSocket, removeListener)).AlwaysReturn();
                fakeit::When(Method(mockSocket, send)).AlwaysReturn(true);

                fakeit::When(Method(mockListener, handshakeComplete)).AlwaysReturn();

                //fakeit::When(Method(mockMsgFactory, serializeMessage)).AlwaysReturn(new cadf::comms::OutputBuffer(1));
                fakeit::When(Method(mockMsgFactory, serializeMessage)).AlwaysDo([&](const auto &packet) {
                    m_sentMsgType = packet.getMessage()->getType();
                    return new cadf::comms::OutputBuffer(1);
                });
            }

            ~SetupMocks() {
                verifyAllMocksChecked();
            }

            void verifySentMessageType(const std::string &expected) {
                BOOST_CHECK_EQUAL(expected, m_sentMsgType);
                m_sentMsgType = "";
            }

            void verifyAllMocksChecked() {
                fakeit::VerifyNoOtherInvocations(mockSocket, mockMsgFactory, mockListener, mockInBuffer);
            }

            // Mocks to use for testing
            fakeit::Mock<cadf::comms::ISocketDataHandler> mockSocket;
            fakeit::Mock<cadf::comms::MessageFactory<MockProtocol>> mockMsgFactory;
            fakeit::Mock<cadf::comms::IHandshakeCompleteListener> mockListener;
            fakeit::Mock<cadf::comms::InputBuffer> mockInBuffer;

        private:
            std::string m_sentMsgType;
    };

    /**
     * Helper fixture to initialize the ProtocolHandler instance that is to be used for the tests
     */
    struct TestFixture: public SetupMocks {

            TestFixture() : SetupMocks(), handshake(&mockSocket.get(), &mockMsgFactory.get()) {
                verifyAllMocksChecked();
            }

            ~TestFixture() {
                verifyAllMocksChecked();
            }

            /** The Handshake to test */
            cadf::comms::ProtocolHandshake<MockProtocol> handshake;
    };

}

BOOST_AUTO_TEST_SUITE(ProtocolHandshake_Test_Suite)

/**
 * Verify the handshake is properly started
 */
    BOOST_FIXTURE_TEST_CASE(StartHandshakeTest, ProtocolHandshakeTest::TestFixture) {
        handshake.start(&mockListener.get());
        fakeit::Verify(Method(mockSocket, addListener).Using(&handshake)).Once();
        fakeit::Verify(Method(mockMsgFactory, serializeMessage)).Once();
        fakeit::Verify(Method(mockSocket, send)).Once();
        verifySentMessageType("HandshakeInitMessage");
    }

    /**
     * Verify the handshake properly processes the response
     */
    BOOST_FIXTURE_TEST_CASE(ReceiveResponseTest, StartHandshakeTest) {
        StartHandshakeTest::test_method();

        cadf::comms::HandshakeResponseMessageV1 *testMessage = new cadf::comms::HandshakeResponseMessageV1( { 123, 321 });
        cadf::comms::MessagePacket *testPacket = new cadf::comms::MessagePacket(testMessage, 0, 0);
        fakeit::When(Method(mockMsgFactory, deserializeMessage).Using(&mockInBuffer.get())).AlwaysReturn(testPacket);

        handshake.messageReceived(&mockInBuffer.get());

        fakeit::Verify(Method(mockMsgFactory, deserializeMessage)).Once();
        fakeit::Verify(Method(mockSocket, removeListener).Using(&handshake)).Once();
        fakeit::Verify(Method(mockMsgFactory, serializeMessage)).Twice();
        fakeit::Verify(Method(mockSocket, send)).Twice();
        verifySentMessageType("HandshakeCompleteMessage");
        fakeit::Verify(Method(mockListener, handshakeComplete).Using(123, 321, &mockSocket.get())).Once();
    }

    BOOST_AUTO_TEST_SUITE_END()
