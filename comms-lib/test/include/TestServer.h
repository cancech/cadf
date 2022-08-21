#ifndef INCLUDE_TESTSERVER_H_
#define INCLUDE_TESTSERVER_H_

#include "comms/bus/LocalBasicBus.h"
#include "comms/network/server/BasicServer.h"
#include "TestMessage.h"

namespace test {

    template<class PROTOCOL>
    class TestServer : public cadf::comms::BasicServer<PROTOCOL, TestMessage1, TestMessage2> {
        public:
            TestServer(const cadf::comms::NetworkInfo &info) : m_bus(), cadf::comms::BasicServer<PROTOCOL, TestMessage1, TestMessage2>(&m_bus, info) {
            }

        protected:
            void registerMessages(cadf::comms::MessageFactory<PROTOCOL> *msgFactory) {
                cadf::comms::MessageRegistry<PROTOCOL, TestMessage3> msgRegistry;
                msgRegistry.registerMessages(msgFactory);
            }

        private:
            cadf::comms::LocalBasicBus m_bus;
    };

}

#endif /* INCLUDE_TESTSERVER_H_ */
