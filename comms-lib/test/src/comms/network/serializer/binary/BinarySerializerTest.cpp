
#define BOOST_TEST_DYN_LINK
#ifdef STAND_ALONE
#   define BOOST_TEST_MODULE Main
#endif
#include <boost/test/unit_test.hpp>
#include <fakeit.hpp>
#include <string>
#include <vector>
#include <map>
#include <iostream>

#include "comms/network/serializer/binary/Serializer.h"
#include "comms/network/Buffer.h"

#include "TestData.h"
#include "TestMessage.h"


namespace SerializerBinaryTest {

    template<typename T, size_t size>
    void checkArrayEqual(const std::array<T, size> &lhs, const std::array<T, size> &rhs) {
        BOOST_CHECK_EQUAL(lhs.size(), rhs.size());
        for (size_t i = 0; i < lhs.size(); i++)
            BOOST_CHECK_EQUAL(lhs[i], rhs[i]);
    }

    template<template<typename, typename > class V, typename T, typename Alloc>
    void checkVectorEqual(const V<T, Alloc> &lhs, const V<T, Alloc> &rhs) {
        BOOST_CHECK_EQUAL(lhs.size(), rhs.size());
        for (size_t i = 0; i < lhs.size(); i++)
            BOOST_CHECK_EQUAL(lhs[i], rhs[i]);
    }

    template<template<typename, typename, typename, typename > class M, typename K, typename T, typename Comp, typename Alloc>
    void checkMapEqual(const M<K, T, Comp, Alloc> &lhs, const M<K, T, Comp, Alloc> &rhs) {
        BOOST_CHECK_EQUAL(lhs.size(), rhs.size());

        for (std::pair<const K&, const T&> lhsElem : lhs) {
            BOOST_CHECK_EQUAL(1, rhs.count(lhsElem.first));
            BOOST_CHECK_EQUAL(lhsElem.second, rhs.at(lhsElem.first));
        }
    }
}

/**
 * Test suite for the Serialization functions
 */
BOOST_AUTO_TEST_SUITE(SerializerBinary_Test_Suite)

/**
 * Verify that the size of PODs is properly determined
 */
    BOOST_AUTO_TEST_CASE(TestPODSize) {
        // Check for scalar PODs
        short s = 10;
        BOOST_CHECK_EQUAL(sizeof(short), cadf::comms::binary::DataSerializer<short>::sizeOf(s));
        int i = 1;
        BOOST_CHECK_EQUAL(sizeof(int), cadf::comms::binary::DataSerializer<int>::sizeOf(i));
        long l = 1234567890;
        BOOST_CHECK_EQUAL(sizeof(long), cadf::comms::binary::DataSerializer<long>::sizeOf(l));
        float f = 0.123;
        BOOST_CHECK_EQUAL(sizeof(float), cadf::comms::binary::DataSerializer<float>::sizeOf(f));
        double d = 99.99;
        BOOST_CHECK_EQUAL(sizeof(double), cadf::comms::binary::DataSerializer<double>::sizeOf(d));
        char c = 'a';
        BOOST_CHECK_EQUAL(sizeof(char), cadf::comms::binary::DataSerializer<char>::sizeOf(c));
        bool b = true;
        BOOST_CHECK_EQUAL(sizeof(bool), cadf::comms::binary::DataSerializer<bool>::sizeOf(b));
        unsigned int ui = 1;
        BOOST_CHECK_EQUAL(sizeof(unsigned int), cadf::comms::binary::DataSerializer<unsigned int>::sizeOf(ui));
        long int li = 321321321;
        BOOST_CHECK_EQUAL(sizeof(long int), cadf::comms::binary::DataSerializer<long int>::sizeOf(li));
        long unsigned int lui = 987987987;
        BOOST_CHECK_EQUAL(sizeof(long unsigned int), cadf::comms::binary::DataSerializer<long unsigned int>::sizeOf(lui));

        // Check for pointer PODs - char* is treated as a string, and so below
        BOOST_CHECK_EQUAL(sizeof(short), cadf::comms::binary::DataSerializer<short*>::sizeOf(&s));
        BOOST_CHECK_EQUAL(sizeof(int), cadf::comms::binary::DataSerializer<int*>::sizeOf(&i));
        BOOST_CHECK_EQUAL(sizeof(long), cadf::comms::binary::DataSerializer<long*>::sizeOf(&l));
        BOOST_CHECK_EQUAL(sizeof(float), cadf::comms::binary::DataSerializer<float*>::sizeOf(&f));
        BOOST_CHECK_EQUAL(sizeof(double), cadf::comms::binary::DataSerializer<double*>::sizeOf(&d));
        BOOST_CHECK_EQUAL(sizeof(bool), cadf::comms::binary::DataSerializer<bool*>::sizeOf(&b));
        BOOST_CHECK_EQUAL(sizeof(unsigned int), cadf::comms::binary::DataSerializer<unsigned int*>::sizeOf(&ui));
        BOOST_CHECK_EQUAL(sizeof(long int), cadf::comms::binary::DataSerializer<long int*>::sizeOf(&li));
        BOOST_CHECK_EQUAL(sizeof(long unsigned int), cadf::comms::binary::DataSerializer<long unsigned int*>::sizeOf(&lui));
    }

    /**
     * Verify that the size of strings is properly determined
     */
    BOOST_AUTO_TEST_CASE(TestStringSize) {
        std::string stdStr = "This is a sample string";
        BOOST_CHECK_EQUAL(sizeof(size_t) + sizeof(char) * stdStr.length(), cadf::comms::binary::DataSerializer<std::string>::sizeOf(stdStr));
    }

    /**
     * Verify that can serialize and deserialize a single value
     */
    BOOST_AUTO_TEST_CASE(SerializeDeserializeSingleValue) {
        int orig = 13579;
        cadf::comms::OutputBuffer out(cadf::comms::binary::DataSerializer<int>::sizeOf(orig));
        BOOST_CHECK_EQUAL(0, out.getDataSize());
        BOOST_CHECK_EQUAL(cadf::comms::binary::DataSerializer<int>::sizeOf(orig), out.getTotalSize());
        cadf::comms::binary::DataSerializer<int>::serialize(orig, &out);
        BOOST_CHECK_EQUAL(cadf::comms::binary::DataSerializer<int>::sizeOf(orig), out.getDataSize());

        cadf::comms::InputBuffer in(out.getData(), out.getDataSize());
        BOOST_CHECK_EQUAL(cadf::comms::binary::DataSerializer<int>::sizeOf(orig), in.getDataSize());
        int copy = cadf::comms::binary::DataSerializer<int>::deserialize(&in);
        BOOST_CHECK_EQUAL(orig, copy);
    }

    /**
     * Verify that can serialize and deserialize a single pointer
     */
    BOOST_AUTO_TEST_CASE(SerializeDeserializeSinglePointer) {
        int orig = 13579;
        cadf::comms::OutputBuffer out(cadf::comms::binary::DataSerializer<int*>::sizeOf(&orig));
        BOOST_CHECK_EQUAL(0, out.getDataSize());
        BOOST_CHECK_EQUAL(cadf::comms::binary::DataSerializer<int*>::sizeOf(&orig), out.getTotalSize());
        cadf::comms::binary::DataSerializer<int*>::serialize(&orig, &out);
        BOOST_CHECK_EQUAL(cadf::comms::binary::DataSerializer<int*>::sizeOf(&orig), out.getDataSize());

        cadf::comms::InputBuffer in(out.getData(), out.getDataSize());
        BOOST_CHECK_EQUAL(cadf::comms::binary::DataSerializer<int*>::sizeOf(&orig), in.getDataSize());
        int *copy = cadf::comms::binary::DataSerializer<int*>::deserialize(&in);
        BOOST_CHECK_EQUAL(orig, *copy);
        delete (copy);
    }

    /**
     * Verify that can serialize and deserialize a std::string
     */
    BOOST_AUTO_TEST_CASE(SerializeDeserializeStdString) {
        std::string orig = "Test string for serialization!";
        cadf::comms::OutputBuffer out(cadf::comms::binary::DataSerializer<std::string>::sizeOf(orig));
        BOOST_CHECK_EQUAL(0, out.getDataSize());
        BOOST_CHECK_EQUAL(cadf::comms::binary::DataSerializer<std::string>::sizeOf(orig), out.getTotalSize());
        cadf::comms::binary::DataSerializer<std::string>::serialize(orig, &out);
        BOOST_CHECK_EQUAL(cadf::comms::binary::DataSerializer<std::string>::sizeOf(orig), out.getDataSize());

        cadf::comms::InputBuffer in(out.getData(), out.getDataSize());
        BOOST_CHECK_EQUAL(cadf::comms::binary::DataSerializer<std::string>::sizeOf(orig), in.getDataSize());
        std::string copy = cadf::comms::binary::DataSerializer<std::string>::deserialize(&in);
        BOOST_CHECK_EQUAL(orig, copy);
    }

    /**
     * Verify that arrays can properly be dealt with
     */
    BOOST_AUTO_TEST_CASE(SerializeDeserializeArrays) {
        std::array<TestData, 5> data;
        data[0] = TestData { 1, 2 };
        data[1] = TestData { 3, 4 };
        data[2] = TestData { 5, 6 };
        data[3] = TestData { 7, 8 };
        data[4] = TestData { 9, 0 };

        size_t calcSize = cadf::comms::binary::DataSerializer<std::array<TestData, 5> >::sizeOf(data);
        BOOST_CHECK_EQUAL(5 * (sizeof(int) + sizeof(double)), calcSize);

        cadf::comms::OutputBuffer out(calcSize);
        cadf::comms::binary::DataSerializer<std::array<TestData, 5>>::serialize(data, &out);

        cadf::comms::InputBuffer in(out.getData(), out.getDataSize());
        std::array<TestData, 5> copy = cadf::comms::binary::DataSerializer<std::array<TestData, 5>>::deserialize(&in);
        SerializerBinaryTest::checkArrayEqual(data, copy);
    }

    /**
     * Verify that dynamic arrays can be properly dealt with
     */
    BOOST_AUTO_TEST_CASE(SerializeDeserializeVector) {
        std::vector<double> data = { 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7 };
        size_t calcSize = cadf::comms::binary::DataSerializer<std::vector<double>>::sizeOf(data);
        BOOST_CHECK_EQUAL(sizeof(size_t) + 7 * sizeof(double), calcSize);

        cadf::comms::OutputBuffer out(calcSize);
        cadf::comms::binary::DataSerializer<std::vector<double>>::serialize(data, &out);

        cadf::comms::InputBuffer in(out.getData(), out.getDataSize());
        std::vector<double> copy = cadf::comms::binary::DataSerializer<std::vector<double>>::deserialize(&in);
        SerializerBinaryTest::checkVectorEqual(data, copy);
    }

    /**
     * Verify that maps can be properly dealt with
     */
    BOOST_AUTO_TEST_CASE(SerializeDeserializeMap) {
        std::map<int, short> data;
        data[123] = 321;
        data[234] = 432;
        data[345] = 543;
        data[456] = 654;
        size_t calcSize = cadf::comms::binary::DataSerializer<std::map<int, short>>::sizeOf(data);
        BOOST_CHECK_EQUAL(sizeof(size_t) + 4 * (sizeof(int) + sizeof(short)), calcSize);

        cadf::comms::OutputBuffer out(calcSize);
        cadf::comms::binary::DataSerializer<std::map<int, short>>::serialize(data, &out);

        cadf::comms::InputBuffer in(out.getData(), out.getDataSize());
        std::map<int, short> copy = cadf::comms::binary::DataSerializer<std::map<int, short>>::deserialize(&in);
        SerializerBinaryTest::checkMapEqual(data, copy);
    }

    /**
     * Verify that can serialize and deserialize multiple values
     */
    BOOST_AUTO_TEST_CASE(SerializeDeserializeMultipleValues) {
        double val1 = 0.01;
        float val2 = 1.11;
        unsigned long val3 = 1234567890;
        bool val4 = false;
        std::string val5 = "A String";
        int *val6 = new int(321);
        short *val7 = new short(0);
        std::array<std::string, 3> val8 = { "String 1", "String 2", "String 3" };
        std::vector<TestData> val9 = { TestData { 1, 2 }, TestData { 3, 0.2 }, TestData { 6, 9.123 }, TestData { 123, 3.21 } };
        std::map<std::string, std::string> val10;
        val10["key1"] = "value1";
        val10["key2"] = "value2";
        val10["key3"] = "value3";
        val10["key4"] = "value4";
        val10["key5"] = "value5";

        size_t dataSize = cadf::comms::binary::DataSerializer<double>::sizeOf(val1) + cadf::comms::binary::DataSerializer<float>::sizeOf(val2) + cadf::comms::binary::DataSerializer<unsigned long>::sizeOf(val3)
                + cadf::comms::binary::DataSerializer<bool>::sizeOf(val4) + cadf::comms::binary::DataSerializer<std::string>::sizeOf(val5) + cadf::comms::binary::DataSerializer<int*>::sizeOf(val6)
                + cadf::comms::binary::DataSerializer<short*>::sizeOf(val7) + cadf::comms::binary::DataSerializer<std::array<std::string, 3>>::sizeOf(val8) + cadf::comms::binary::DataSerializer<std::vector<TestData>>::sizeOf(val9)
                + cadf::comms::binary::DataSerializer<std::map<std::string, std::string>>::sizeOf(val10);
        cadf::comms::OutputBuffer out(dataSize);
        BOOST_CHECK_EQUAL(0, out.getDataSize());
        BOOST_CHECK_EQUAL(dataSize, out.getTotalSize());
        cadf::comms::binary::DataSerializer<double>::serialize(val1, &out);
        cadf::comms::binary::DataSerializer<float>::serialize(val2, &out);
        cadf::comms::binary::DataSerializer<unsigned long>::serialize(val3, &out);
        cadf::comms::binary::DataSerializer<bool>::serialize(val4, &out);
        cadf::comms::binary::DataSerializer<std::string>::serialize(val5, &out);
        cadf::comms::binary::DataSerializer<int*>::serialize(val6, &out);
        cadf::comms::binary::DataSerializer<short*>::serialize(val7, &out);
        cadf::comms::binary::DataSerializer<std::array<std::string, 3>>::serialize(val8, &out);
        cadf::comms::binary::DataSerializer<std::vector<TestData>>::serialize(val9, &out);
        cadf::comms::binary::DataSerializer<std::map<std::string, std::string>>::serialize(val10, &out);
        BOOST_CHECK_EQUAL(dataSize, out.getDataSize());

        cadf::comms::InputBuffer in(out.getData(), out.getDataSize());
        BOOST_CHECK_EQUAL(dataSize, in.getDataSize());
        BOOST_CHECK_EQUAL(out.getData(), in.getData());
        double val1Copy = cadf::comms::binary::DataSerializer<double>::deserialize(&in);
        float val2Copy = cadf::comms::binary::DataSerializer<float>::deserialize(&in);
        unsigned long val3Copy = cadf::comms::binary::DataSerializer<unsigned long>::deserialize(&in);
        bool val4Copy = cadf::comms::binary::DataSerializer<bool>::deserialize(&in);
        std::string val5Copy = cadf::comms::binary::DataSerializer<std::string>::deserialize(&in);
        int *val6Copy = cadf::comms::binary::DataSerializer<int*>::deserialize(&in);
        short *val7Copy = cadf::comms::binary::DataSerializer<short*>::deserialize(&in);
        std::array<std::string, 3> val8Copy = cadf::comms::binary::DataSerializer<std::array<std::string, 3>>::deserialize(&in);
        std::vector<TestData> val9Copy = cadf::comms::binary::DataSerializer<std::vector<TestData>>::deserialize(&in);
        std::map<std::string, std::string> val10Copy = cadf::comms::binary::DataSerializer<std::map<std::string, std::string>>::deserialize(&in);

        BOOST_CHECK_EQUAL(val1, val1Copy);
        BOOST_CHECK_EQUAL(val2, val2Copy);
        BOOST_CHECK_EQUAL(val3, val3Copy);
        BOOST_CHECK_EQUAL(val4, val4Copy);
        BOOST_CHECK_EQUAL(val5, val5Copy);
        BOOST_CHECK_EQUAL(*val6, *val6Copy);
        BOOST_CHECK_EQUAL(*val7, *val7Copy);
        SerializerBinaryTest::checkArrayEqual(val8, val8Copy);
        SerializerBinaryTest::checkVectorEqual(val9, val9Copy);
        SerializerBinaryTest::checkMapEqual(val10, val10Copy);

        delete (val6);
        delete (val7);
        delete (val6Copy);
        delete (val7Copy);
    }

    /**
     * Verify that can serialize and deserialize a Data Message
     */
    BOOST_AUTO_TEST_CASE(SerializeDeserializeDataMessageValues) {
        TestData data = {987, 3.21};
        TestMessage3 msg(data);

        cadf::comms::binary::MessageSerializer<TestData> serializer(&msg, 10 ,20);
        size_t expectedSize = sizeof(size_t) + (sizeof(char) * msg.getType().length()) + 3 * sizeof(int) + sizeof(double);
        BOOST_CHECK_EQUAL(expectedSize, serializer.getSize());
        cadf::comms::OutputBuffer outBuffer(serializer.getSize());
        serializer.serialize(&outBuffer);

        cadf::comms::InputBuffer inBuffer(outBuffer.getData(), outBuffer.getDataSize());
        cadf::comms::binary::MessageDeserializer deserializer(&inBuffer);
        BOOST_CHECK_EQUAL("TestMessage3", deserializer.getMessageType());
        BOOST_CHECK_EQUAL(10, deserializer.getRecipientType());
        BOOST_CHECK_EQUAL(20, deserializer.getRecipientInstance());

        TestData newData = deserializer.getData<TestData>();
        BOOST_CHECK_EQUAL(data, newData);
    }

    /**
     * Verify that can serialize and deserialize a Message Packet
     */
    BOOST_AUTO_TEST_CASE(FullSerializationDeserializationTest) {
        TestData data = {123, 4.56};
        TestMessage1 sent(data);

        // Serialize the packet information
        cadf::comms::ISerializerFactory *factory = cadf::comms::binary::BinaryProtocol::createSerializerFactory(&sent);
        cadf::comms::ISerializer *serializer = factory->buildSerializer(&sent, 31, 79);
        cadf::comms::OutputBuffer out(serializer->getSize());
        serializer->serialize(&out);
        delete(serializer);

        // Deserialize the information
        cadf::comms::InputBuffer in(out.getData(), out.getDataSize());
        cadf::comms::IDeserializer *deserializer = cadf::comms::binary::BinaryProtocol::createDeserializer(&in);
        BOOST_CHECK_EQUAL("TestMessage1", deserializer->getMessageType());
        BOOST_CHECK_EQUAL(31, deserializer->getRecipientType());
        BOOST_CHECK_EQUAL(79, deserializer->getRecipientInstance());

        TestMessage1 received;
        factory->deserializeTo(&received, deserializer);
        BOOST_CHECK_EQUAL(123, received.getData().val1);
        BOOST_CHECK_EQUAL(4.56, received.getData().val2);


        delete(deserializer);
        delete(factory);
    }

    BOOST_AUTO_TEST_SUITE_END()
