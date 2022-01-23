#define BOOST_TEST_DYN_LINK
#ifdef STAND_ALONE
#   define BOOST_TEST_MODULE Main
#endif
#include <boost/test/unit_test.hpp>
#include <fakeit.hpp>

#include "comms/network/serializer/local/Serializer.h"
#include "comms/network/NetworkException.h"

#include "TestMessage.h"

/**
 * Test suite for the Local Protocol
 */
BOOST_AUTO_TEST_SUITE(LocalProtocol_Test_Suite)

/**
 * Verify that can serialize and deserialize some values to/from the buffer
 */
    BOOST_AUTO_TEST_CASE(TestCreateSerializerFactory) {
        TestMessage1 msg;
        cadf::comms::ISerializerFactory *factory = cadf::comms::local::LocalProtocol::createSerializerFactory(&msg);
        BOOST_CHECK(factory != NULL);
        BOOST_CHECK(dynamic_cast<cadf::comms::local::LocalSerializerFactory*>(factory) != NULL);
        delete (factory);
    }

    /**
     * Verify that calling anything else results in an exception
     */
    BOOST_AUTO_TEST_CASE(TestLocalProtocolException) {
        cadf::comms::InputBuffer in("", 0);
        BOOST_CHECK_THROW(cadf::comms::local::LocalProtocol::createDeserializer(&in), cadf::comms::ProtocolException);

        TestMessage1 msg;
        fakeit::Mock<cadf::comms::IDeserializer> mockDeserializer;
        cadf::comms::local::LocalSerializerFactory factory;
        BOOST_CHECK_THROW(factory.buildSerializer(&msg, 1, 1), cadf::comms::ProtocolException);
        BOOST_CHECK_THROW(factory.deserializeTo(&msg, &mockDeserializer.get()), cadf::comms::ProtocolException);
        fakeit::VerifyNoOtherInvocations(mockDeserializer);
    }

    BOOST_AUTO_TEST_SUITE_END()
