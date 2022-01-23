#ifndef INCLUDE_TESTNETNODE_H_
#define INCLUDE_TESTNETNODE_H_

#include "comms/node/Node.h"
#include "comms/connection/ClientConnection.h"
#include "comms/network/client/BasicClient.h"
#include "comms/network/socket/TcpClientSocket.h"
#include "comms/network/NetworkInfo.h"

#include <boost/test/unit_test.hpp>
#include "TestMessage.h"

namespace test {

    template<class PROTOCOL>
    class TestNetNode {
        public:
            TestNetNode(int type, int instance, const cadf::comms::NetworkInfo &info) : m_msgFactory(), m_clientSocket(info, 1024), m_client(&m_clientSocket),
                    m_clientConnection(type, instance, &m_msgFactory, &m_client), m_clientNode(&m_clientConnection) {
                cadf::comms::MessageRegistry<PROTOCOL, TestMessage1, TestMessage2, TestMessage3> msgRegistry;
                msgRegistry.registerMessages(&m_msgFactory);
            }

            virtual ~TestNetNode() {
            }

            virtual bool connect() {
                return m_clientNode.connect();
            }

            virtual bool disconnect() {
                return m_clientNode.disconnect();
            }

            virtual bool isConnected() {
                return m_clientNode.isConnected();
            }

            virtual void addProcessor(cadf::comms::IProcessor *processor) {
                m_clientNode.addProcessor(processor);
            }

            virtual bool sendMessage(cadf::comms::IMessage *msg, int type, int instance) {
                return m_clientNode.sendMessage(msg, type, instance);
            }

        private:
            cadf::comms::MessageFactory<PROTOCOL> m_msgFactory;
            cadf::comms::TcpClientSocket m_clientSocket;
            cadf::comms::BasicClient m_client;
            cadf::comms::ClientConnection<PROTOCOL> m_clientConnection;
            cadf::comms::Node m_clientNode;
    };

    class TestMessage1Processor: public cadf::comms::MessageProcessor<TestMessage1> {
        public:
            virtual void processMessage(const TestMessage1 *msg) {
                numReceived++;
                lastVal1 = msg->getData().val1;
                lastVal2 = msg->getData().val2;
            }

            void verifyState(int expectedReceived, int expectLastVal1, double expectedLastVal2) {
                BOOST_CHECK_EQUAL(expectedReceived, numReceived);
                BOOST_CHECK_EQUAL(expectLastVal1, lastVal1);
                lastVal1 = -1;
                BOOST_CHECK_EQUAL(expectedLastVal2, lastVal2);
                lastVal2 = -2;
            }

        private:
            int numReceived = 0;
            int lastVal1 = -1;
            double lastVal2 = -2;
    };

}

#endif /* INCLUDE_TESTNETNODE_H_ */
