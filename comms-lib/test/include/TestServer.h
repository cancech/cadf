#ifndef INCLUDE_TESTSERVER_H_
#define INCLUDE_TESTSERVER_H_

#include "comms/network/BasicNodeBusServer.h"
#include "comms/bus/BasicBus.h"
#include "TestMessage.h"

namespace test {

    template<class PROTOCOL>
    class TestServer : public cadf::comms::BasicNodeBusServer<PROTOCOL, TestMessage1, TestMessage2> {
        public:
            TestServer(const cadf::comms::NetworkInfo &info) : m_bus(), cadf::comms::BasicNodeBusServer<PROTOCOL, TestMessage1, TestMessage2>(&m_bus, info, 128) {
            }

        protected:
            void registerMessages(cadf::comms::MessageFactory<PROTOCOL> *msgFactory) {
                cadf::comms::MessageRegistry<PROTOCOL, TestMessage3> msgRegistry;
                msgRegistry.registerMessages(msgFactory);
            }

        private:
            cadf::comms::BasicBus m_bus;
    };

}

#endif /* INCLUDE_TESTSERVER_H_ */
