#define BOOST_TEST_DYN_LINK
#ifdef STAND_ALONE
#   define BOOST_TEST_MODULE Main
#endif
#include <boost/test/unit_test.hpp>
#include <fakeit.hpp>

#include "comms/network/client/BasicClient.h"

namespace BasicClientTest {

    /*
     * Helper fixture for initializing and preparing all of the necessary mocks
     */
    struct SetupMocks {

            SetupMocks() {
                whenSocketIsConnectedReturn(false);
                fakeit::When(Method(mockSocket, connect)).AlwaysReturn(true);
                fakeit::When(Method(mockSocket, disconnect)).AlwaysReturn(true);
                fakeit::Fake(Method(mockSocket, addMessageListener));
                fakeit::Fake(Method(mockSocket, removeMessageListener));
            }

            ~SetupMocks() {
                verifyAllMocksChecked();
            }

            void whenSocketIsConnectedReturn(bool toReturn) {
                fakeit::When(Method(mockSocket, isConnected)).AlwaysReturn(toReturn);
            }

            void verifyAllMocksChecked() {
                fakeit::VerifyNoOtherInvocations(mockSocket, mockListener, mockListener2, mockOutBuffer);
            }

            // Mocks to use for testing
            fakeit::Mock<cadf::comms::TcpClientSocket> mockSocket;
            fakeit::Mock<cadf::comms::ISocketMessageReceivedListener> mockListener;
            fakeit::Mock<cadf::comms::ISocketMessageReceivedListener> mockListener2;
            fakeit::Mock<cadf::comms::OutputBuffer> mockOutBuffer;
    };

    /**
     * Helper fixture to initialize the BasicClient instance that is to be used for the tests
     */
    struct TestFixture: public SetupMocks {

            TestFixture() : SetupMocks() {
                verifyAllMocksChecked();
                client = new cadf::comms::BasicClient(&mockSocket.get());
            }

            ~TestFixture() {
                delete (client);
                if (isConnected)
                    verifyDisconnectState(1, 1, registeredListener);
                else
                    verifyIsConnectedCalled();
                verifyAllMocksChecked();
            }

            void connectClient(int timesConnected, int timesListenerAdded, cadf::comms::ISocketMessageReceivedListener *expectedListener) {
                BOOST_CHECK(client->connect());
                verifyIsConnectedCalled();
                fakeit::Verify(Method(mockSocket, connect)).Exactly(timesConnected);
                fakeit::Verify(Method(mockSocket, addMessageListener).Using(expectedListener)).Exactly(timesListenerAdded);

                whenSocketIsConnectedReturn(true);
                registeredListener = expectedListener;
                isConnected = true;
            }

            void disconnectClient(int timesDisonnected, int timesListenerRemoved, cadf::comms::ISocketMessageReceivedListener *expectedListener) {
                BOOST_CHECK(client->disconnect());
                verifyDisconnectState(timesDisonnected, timesListenerRemoved, expectedListener);
            }

            void verifyDisconnectState(int timesDisonnected, int timesListenerRemoved, cadf::comms::ISocketMessageReceivedListener *expectedListener) {
                verifyIsConnectedCalled();
                fakeit::Verify(Method(mockSocket, disconnect)).Exactly(timesDisonnected);
                if (expectedListener != NULL)
                    fakeit::Verify(Method(mockSocket, removeMessageListener).Using(expectedListener)).Exactly(timesListenerRemoved);

                whenSocketIsConnectedReturn(false);
                isConnected = true;
            }

            void registerListener(cadf::comms::ISocketMessageReceivedListener *expectedListener) {
                client->setListener(expectedListener);
                verifyIsConnectedCalled();
                if (isConnected)
                    fakeit::Verify(Method(mockSocket, addMessageListener).Using(expectedListener)).Once();

                registeredListener = expectedListener;
            }

            void verifyIsConnectedCalled() {
                fakeit::Verify(Method(mockSocket, isConnected)).Exactly(++timesIsConnectedCalled);
            }

            // Helper mechanisms
            int timesIsConnectedCalled = 0;
            bool isConnected = false;
            cadf::comms::ISocketMessageReceivedListener *registeredListener = NULL;

            /** The client to test */
            cadf::comms::BasicClient *client;
    };

}

BOOST_AUTO_TEST_SUITE(BasicClient_Test_Suite)

/**
 * Verify that can connect without a registered listener
 */
    BOOST_FIXTURE_TEST_CASE(ConnectWithoutRegisteredListenerTest, BasicClientTest::TestFixture) {
        BOOST_CHECK(!client->isConnected());
        verifyIsConnectedCalled();

        connectClient(1, 0, NULL);

        BOOST_CHECK(client->isConnected());
        verifyIsConnectedCalled();
    }

    /**
     * Verify that can register a listener when not connected
     */
    BOOST_FIXTURE_TEST_CASE(RegisterListenerWhenDisconnectedTest, BasicClientTest::TestFixture) {
        BOOST_CHECK(!client->isConnected());
        verifyIsConnectedCalled();

        client->setListener(&mockListener.get());
        verifyIsConnectedCalled();
    }

    /**
     * Verify that can connect without a registered listener
     */
    BOOST_FIXTURE_TEST_CASE(ConnectWithRegisteredListenerTest, RegisterListenerWhenDisconnectedTest) {
        RegisterListenerWhenDisconnectedTest::test_method();
        BOOST_CHECK(!client->isConnected());
        verifyIsConnectedCalled();

        connectClient(1, 1, &mockListener.get());

        BOOST_CHECK(client->isConnected());
        verifyIsConnectedCalled();
    }

    /**
     * Verify that can register a listener when connected
     */
    BOOST_FIXTURE_TEST_CASE(RegisterListenerWhenConnectedTest, ConnectWithoutRegisteredListenerTest) {
        ConnectWithoutRegisteredListenerTest::test_method();
        BOOST_CHECK(client->isConnected());
        verifyIsConnectedCalled();

        registerListener(&mockListener.get());
    }

    /**
     * Verify that can change the registered listener when connected
     */
    BOOST_FIXTURE_TEST_CASE(RegisterNewListenerWhenConnectedTest, RegisterListenerWhenConnectedTest) {
        RegisterListenerWhenConnectedTest::test_method();
        BOOST_CHECK(client->isConnected());
        verifyIsConnectedCalled();

        registerListener(&mockListener2.get());
        fakeit::Verify(Method(mockSocket, removeMessageListener).Using(&mockListener.get())).Once();
    }

    /**
     * Verify that can change the registered listener when not connected
     */
    BOOST_FIXTURE_TEST_CASE(RegisterNewListenerWhenDisonnectedTest, RegisterListenerWhenDisconnectedTest) {
        RegisterListenerWhenDisconnectedTest::test_method();
        BOOST_CHECK(!client->isConnected());
        verifyIsConnectedCalled();

        client->setListener(&mockListener2.get());
        verifyIsConnectedCalled();
    }

    /**
     * Verify that can Disconnect when connected without a listener
     */
    BOOST_FIXTURE_TEST_CASE(DisconnectWhenConnectedWithoutListenerTest, ConnectWithoutRegisteredListenerTest) {
        ConnectWithoutRegisteredListenerTest::test_method();
        BOOST_CHECK(client->isConnected());
        verifyIsConnectedCalled();

        disconnectClient(1, 0, NULL);
    }

    /**
     * Verify that can Disconnect when connected with a listener
     */
    BOOST_FIXTURE_TEST_CASE(DisconnectWhenConnectedWithListenerTest, RegisterListenerWhenConnectedTest) {
        RegisterListenerWhenConnectedTest::test_method();
        BOOST_CHECK(client->isConnected());
        verifyIsConnectedCalled();

        disconnectClient(1, 1, &mockListener.get());
    }

    /**
     * Verify that a failure to connect is properly handled
     */
    BOOST_FIXTURE_TEST_CASE(FailureToConnectTest, RegisterListenerWhenDisconnectedTest) {
        RegisterListenerWhenDisconnectedTest::test_method();
        fakeit::When(Method(mockSocket, connect)).AlwaysReturn(false);

        BOOST_CHECK(!client->isConnected());
        verifyIsConnectedCalled();

        BOOST_CHECK(!client->connect());
        verifyIsConnectedCalled();
        fakeit::Verify(Method(mockSocket, connect)).Once();

        // Clear the failure
        fakeit::When(Method(mockSocket, connect)).AlwaysReturn(true);
    }

    /**
     * Verify that a failure to disconnect is properly handled
     */
    BOOST_FIXTURE_TEST_CASE(FailureToDisconnectTest, ConnectWithRegisteredListenerTest) {
        ConnectWithRegisteredListenerTest::test_method();
        fakeit::When(Method(mockSocket, disconnect)).AlwaysReturn(false);

        BOOST_CHECK(client->isConnected());
        verifyIsConnectedCalled();

        BOOST_CHECK(!client->disconnect());
        verifyIsConnectedCalled();
        fakeit::Verify(Method(mockSocket, removeMessageListener).Using(&mockListener.get())).Once();
        fakeit::Verify(Method(mockSocket, disconnect)).Once();

        // Clear the failure
        fakeit::When(Method(mockSocket, disconnect)).AlwaysReturn(true);
        whenSocketIsConnectedReturn(false);
    }

    /**
     * Verify that connecting when connected does nothing
     */
    BOOST_FIXTURE_TEST_CASE(ConnectWhenConnectedTest, ConnectWithRegisteredListenerTest) {
        ConnectWithRegisteredListenerTest::test_method();
        BOOST_CHECK(client->isConnected());
        verifyIsConnectedCalled();

        BOOST_CHECK(client->connect());
        verifyIsConnectedCalled();
    }

    /**
     * Verify that disconnecting when disconnected does nothing
     */
    BOOST_FIXTURE_TEST_CASE(DisconnectWhenDisconnectedTest, RegisterListenerWhenDisconnectedTest) {
        RegisterListenerWhenDisconnectedTest::test_method();
        BOOST_CHECK(!client->isConnected());
        verifyIsConnectedCalled();

        BOOST_CHECK(client->disconnect());
        verifyIsConnectedCalled();
    }

    /**
     * Verify that attempting to send a message works as expected
     */
    BOOST_FIXTURE_TEST_CASE(SendMessageTest, BasicClientTest::TestFixture) {
        fakeit::When(Method(mockSocket, send)).AlwaysReturn(true);
        BOOST_CHECK(client->send(&mockOutBuffer.get()));
        fakeit::Verify(Method(mockSocket, send).Using(&mockOutBuffer.get())).Exactly(1);

        fakeit::When(Method(mockSocket, send)).AlwaysReturn(false);
        BOOST_CHECK(!client->send(&mockOutBuffer.get()));
        fakeit::Verify(Method(mockSocket, send).Using(&mockOutBuffer.get())).Exactly(2);
    }

    BOOST_AUTO_TEST_SUITE_END()
