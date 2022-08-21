#ifndef INCLUDE_TESTNETNODE_H_
#define INCLUDE_TESTNETNODE_H_

#include "comms/network/BasicNodeClient.h"

#include <boost/test/unit_test.hpp>
#include "TestMessage.h"

namespace test {

    template<class PROTOCOL>
    class TestNetNode : public cadf::comms::BasicNodeClient<PROTOCOL, TestMessage1, TestMessage2, TestMessage3> {
        public:
            TestNetNode(int type, int instance, const cadf::comms::NetworkInfo &info) :
                cadf::comms::BasicNodeClient<PROTOCOL, TestMessage1, TestMessage2, TestMessage3>(type, instance, info, 1024) {
            }
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
