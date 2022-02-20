#include "comms/network/serializer/binary/Serializer.h"
#include "comms/network/Buffer.h"
#include "TestData.h"

#include "comms/network/serializer/dom/BaseSerializer.h"
#include "comms/message/MessageFactory.h"
#include "comms/message/Message.h"

bool operator==(const TestData& lhs, const TestData& rhs) {
    return lhs.val1 == rhs.val1 &&
            lhs.val2 == rhs.val2;
}

std::ostream& operator<<(std::ostream& stream, const TestData& data) {
    stream << "[" << data.val1 << ", " << data.val2 << "]";
    return stream;
}

template<>
size_t cadf::comms::binary::sizeOfData<TestData>(const TestData &data) {
    return DataSerializer<int>::sizeOf(data.val1) + DataSerializer<double>::sizeOf(data.val2);
}

template<>
void cadf::comms::binary::serializeData<TestData>(const TestData& data, cadf::comms::OutputBuffer *buffer) {
    buffer->append(data.val1, sizeof(int));
    buffer->append(data.val2, sizeof(double));
}

template<>
TestData cadf::comms::binary::deserializeData<TestData>(cadf::comms::InputBuffer *buffer) {
    TestData data;
    data.val1 = buffer->retrieveNext<int>(sizeof(int));
    data.val2 = buffer->retrieveNext<double>(sizeof(double));
    return data;
}

template<>
cadf::dom::DomNode cadf::comms::dom::buildTree<TestData>(const TestData &data) {
    cadf::dom::DomNode node = cadf::dom::buildNode("val1", data.val1);
    node["val2"] = data.val2;
    return node;
}

template<>
TestData cadf::comms::dom::loadFromTree<TestData>(const cadf::dom::DomNode &root) {
    TestData data;
    data.val1 = root["val1"];
    data.val2 = root["val2"];
    return data;
}
