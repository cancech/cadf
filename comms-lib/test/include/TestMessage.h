#ifndef INCLUDE_TESTMESSAGE_H_
#define INCLUDE_TESTMESSAGE_H_

#include "comms/message/Message.h"
#include "TestData.h"

class TestMessage1: public cadf::comms::AbstractDataMessage<TestData> {

public:
    TestMessage1(): cadf::comms::AbstractDataMessage<TestData>("TestMessage1", TestData()) {}
    TestMessage1(const TestData &data) : cadf::comms::AbstractDataMessage<TestData>("TestMessage1", data) {}

    AbstractDataMessage<TestData>* newInstance() const {
        return new TestMessage1(m_data);
    }

    bool equals(const TestMessage1* rhs) {
        return getType() == rhs->getType() && m_data == rhs->m_data;
    }
};

class TestMessage2: public cadf::comms::AbstractDataMessage<TestData> {

public:
    TestMessage2(): cadf::comms::AbstractDataMessage<TestData>("TestMessage2", TestData()) {}
    TestMessage2(const TestData &data) : cadf::comms::AbstractDataMessage<TestData>("TestMessage2", data) {}

    AbstractDataMessage<TestData>* newInstance() const {
        return new TestMessage2(m_data);
    }

    bool equals(const TestMessage2* rhs) {
        return getType() == rhs->getType() && m_data == rhs->m_data;
    }
};

class TestMessage3: public cadf::comms::AbstractDataMessage<TestData> {

public:
    TestMessage3(): cadf::comms::AbstractDataMessage<TestData>("TestMessage3", TestData()) {}
    TestMessage3(const TestData &data) : cadf::comms::AbstractDataMessage<TestData>("TestMessage3", data) {}

    AbstractDataMessage<TestData>* newInstance() const {
        return new TestMessage3(m_data);
    }

    bool equals(const TestMessage3* rhs) {
        return getType() == rhs->getType() && m_data == rhs->m_data;
    }
};

#endif /* INCLUDE_TESTMESSAGE_H_ */
