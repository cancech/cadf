#define BOOST_TEST_DYN_LINK
#ifdef STAND_ALONE
#   define BOOST_TEST_MODULE Main
#endif
#include <boost/test/unit_test.hpp>

#include "comms/network/NetworkInfo.h"

/**
 * Unit test for the Network Info
 */
BOOST_AUTO_TEST_SUITE(NetworkInfo_Test_Suite)

/**
 * Verify that the network information is properly handled
 */
    BOOST_AUTO_TEST_CASE(VerifyIPv4LocalHost) {
        cadf::comms::NetworkInfo info = { cadf::comms::NetworkInfo::IPv4, "127.0.0.1", 123 };
        BOOST_CHECK_EQUAL(SOCK_STREAM, info.typeAsSocketType());
        BOOST_CHECK_EQUAL(AF_INET, info.ipVersionAsAddressFamily());

        sockaddr_in addr = info.toSocketAddress();
        BOOST_CHECK_EQUAL(AF_INET, addr.sin_family);
        BOOST_CHECK_EQUAL(htons(123), addr.sin_port);

        in_addr binAddr;
        inet_pton(AF_INET, "127.0.0.1", &binAddr);
        BOOST_CHECK_EQUAL(binAddr.s_addr, addr.sin_addr.s_addr);
    }

    /**
     * Verify that the network information is properly handled
     */
    BOOST_AUTO_TEST_CASE(VerifyInfoIPv6) {
        cadf::comms::NetworkInfo info = { cadf::comms::NetworkInfo::IPv6, "fe80::4bb6:4c2:c581:7da7", 741 };
        BOOST_CHECK_EQUAL(SOCK_STREAM, info.typeAsSocketType());
        BOOST_CHECK_EQUAL(AF_INET6, info.ipVersionAsAddressFamily());

        sockaddr_in addr = info.toSocketAddress();
        BOOST_CHECK_EQUAL(AF_INET6, addr.sin_family);
        BOOST_CHECK_EQUAL(htons(741), addr.sin_port);

        in_addr binAddr;
        inet_pton(AF_INET6, "fe80::4bb6:4c2:c581:7da7", &binAddr);
        BOOST_CHECK_EQUAL(binAddr.s_addr, addr.sin_addr.s_addr);
    }

    /**
     * Verify that the network information is properly handled
     */
    BOOST_AUTO_TEST_CASE(VerifyInfoEmptyIPv4) {
        cadf::comms::NetworkInfo info = { cadf::comms::NetworkInfo::IPv4, "", 5157 };
        BOOST_CHECK_EQUAL(SOCK_STREAM, info.typeAsSocketType());
        BOOST_CHECK_EQUAL(AF_INET, info.ipVersionAsAddressFamily());

        sockaddr_in addr = info.toSocketAddress();
        BOOST_CHECK_EQUAL(AF_INET, addr.sin_family);
        BOOST_CHECK_EQUAL(htons(5157), addr.sin_port);
        BOOST_CHECK_EQUAL(INADDR_ANY, addr.sin_addr.s_addr);
    }

    /**
     * Verify that the network information is properly handled
     */
    BOOST_AUTO_TEST_CASE(VerifyInfoEmptyIPv6) {
        cadf::comms::NetworkInfo info = { cadf::comms::NetworkInfo::IPv6, "", 8451 };
        BOOST_CHECK_EQUAL(SOCK_STREAM, info.typeAsSocketType());
        BOOST_CHECK_EQUAL(AF_INET6, info.ipVersionAsAddressFamily());

        sockaddr_in addr = info.toSocketAddress();
        BOOST_CHECK_EQUAL(AF_INET6, addr.sin_family);
        BOOST_CHECK_EQUAL(htons(8451), addr.sin_port);
        BOOST_CHECK_EQUAL(INADDR_ANY, addr.sin_addr.s_addr);
    }

    BOOST_AUTO_TEST_SUITE_END()
