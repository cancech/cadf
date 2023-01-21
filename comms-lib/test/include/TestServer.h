#ifndef INCLUDE_TESTSERVER_H_
#define INCLUDE_TESTSERVER_H_

#include "comms/network/BasicNodeBusServer.h"
#include "comms/bus/BasicBus.h"
#include "TestMessage.h"

namespace test {

    template<class PROTOCOL>
    class TestServer : public cadf::comms::BasicNodeBusServer<PROTOCOL, TestMessage1, TestMessage2> {
        public:
            TestServer(cadf::comms::MessageFactory<PROTOCOL> *msgFactory, const cadf::comms::NetworkInfo &info) : m_bus(),
                    m_handshakeFactory(256, msgFactory), m_handshakeHandler(&m_handshakeFactory),
                    cadf::comms::BasicNodeBusServer<PROTOCOL, TestMessage1, TestMessage2>(&m_handshakeHandler, &m_bus, info, 128) {
                cadf::comms::MessageRegistry<PROTOCOL, TestMessage3> msgRegistry;
                msgRegistry.registerMessages(msgFactory);
            }

        private:
            cadf::comms::BasicBus m_bus;
            cadf::comms::ProtocolHandshakeFactory<PROTOCOL> m_handshakeFactory;
            cadf::comms::HandshakeHandler m_handshakeHandler;
    };

}

#endif /* INCLUDE_TESTSERVER_H_ */
