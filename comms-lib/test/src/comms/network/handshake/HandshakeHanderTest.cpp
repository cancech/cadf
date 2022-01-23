#define BOOST_TEST_DYN_LINK
#ifdef STAND_ALONE
#   define BOOST_TEST_MODULE Main
#endif
#include <boost/test/unit_test.hpp>
#include <fakeit.hpp>

#include "comms/network/handshake/HandshakeHandler.h"

namespace HandshakeHandlerTest {

    /**
     * Test variation to expose the protected cleanup method
     */
    class TestHandshakeHandler: public cadf::comms::HandshakeHandler {
        public:
            TestHandshakeHandler(cadf::comms::IHandshakeFactory *handshakeFactory) : cadf::comms::HandshakeHandler(handshakeFactory) {
            }

            virtual ~TestHandshakeHandler() = default;

            virtual void cleanup(cadf::comms::HandshakeTerminator *done) {
                cadf::comms::HandshakeHandler::cleanup(done);
            }
    };

    /*
     * Helper fixture for initializing and preparing all of the necessary mocks
     */
    struct SetupMocks {

            SetupMocks() {
                fakeit::When(Method(mockFactory, create)).AlwaysReturn(&mockHandshake.get());
                fakeit::Fake(Method(mockListener, handshakeComplete));

                fakeit::When(Method(mockHandshake, start)).AlwaysDo([&](auto *listener) {
                    terminator = listener;
                });
            }

            ~SetupMocks() {
                verifyAllMocksChecked();
            }

            void verifyAllMocksChecked() {
                fakeit::VerifyNoOtherInvocations(mockFactory, mockListener);
            }

            // Mocks to use for testing
            fakeit::Mock<cadf::comms::IHandshakeFactory> mockFactory;
            fakeit::Mock<cadf::comms::IHandshakeCompleteListener> mockListener;
            fakeit::Mock<cadf::comms::IHandshake> mockHandshake;
            fakeit::Mock<cadf::comms::ISocketDataHandler> mockSocket;

            // Instances created during the test
            cadf::comms::IHandshakeCompleteListener *terminator = NULL;
    };

    /**
     * Helper fixture to initialize the HandshakeHandler instance that is to be used for the tests
     */
    struct TestFixture: public SetupMocks {

            TestFixture() : SetupMocks(), handler(&mockFactory.get()) {
                verifyAllMocksChecked();
            }

            ~TestFixture() {
                verifyAllMocksChecked();
            }

            /** The Handshake to test */
            cadf::comms::HandshakeHandler handler;
    };
}

/**
 * Test to verify that the handshake handler can properly handle a handshake
 */
BOOST_AUTO_TEST_SUITE(HandshakeHandler_Test_Suite)

/**
 * Verify that a handshake can be performed successfully.
 */
    BOOST_FIXTURE_TEST_CASE(PerformHandshakeTest, HandshakeHandlerTest::TestFixture) {
        handler.performHandshake(123, &mockListener.get());
        fakeit::Verify(Method(mockFactory, create).Using(123)).Once();
        fakeit::Verify(Method(mockHandshake, start)).Once();
        BOOST_CHECK(terminator != NULL);

        terminator->handshakeComplete(21, 53, &mockSocket.get());
        fakeit::Verify(Method(mockListener, handshakeComplete).Using(21, 53, &mockSocket.get())).Once();
    }

    /**
     * Verify that the handshake terminator properly terminates
     */
    BOOST_FIXTURE_TEST_CASE(HandshakeTerminatorTest, HandshakeHandlerTest::SetupMocks) {
        fakeit::Mock<HandshakeHandlerTest::TestHandshakeHandler> mockHandler;
        fakeit::Fake(Method(mockHandler, cleanup));

        cadf::comms::HandshakeTerminator *terminator = new cadf::comms::HandshakeTerminator(&mockHandler.get(), &mockHandshake.get(), &mockListener.get());
        terminator->handshakeComplete(82, 523, &mockSocket.get());
        fakeit::Verify(Method(mockListener, handshakeComplete).Using(82, 523, &mockSocket.get())).Once();
        fakeit::Verify(Method(mockHandler, cleanup).Using(terminator)).Once();
    }

    BOOST_AUTO_TEST_SUITE_END()
