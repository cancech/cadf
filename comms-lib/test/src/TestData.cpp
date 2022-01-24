#include "comms/network/serializer/binary/Serializer.h"
#include "comms/network/serializer/json/Serializer.h"
#include "comms/network/Buffer.h"
#include "TestData.h"
#include "comms/message/MessageFactory.h"
#include "comms/message/Message.h"
#include "dom/JSONBuilder.h"

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
size_t cadf::comms::json::sizeOfJSON<TestData>(const TestData &data) {
    return numOfCharsKey("val1") + numOfCharsValue(data.val1) + numOfCharsKey("val2") + numOfCharsValue(data.val2)
            + 2 // the {} delimiting the JSON structure
            + 1; // The comma between the values
}

template<>
void cadf::comms::json::populateBuilder<TestData>(const TestData &data, cadf::dom::JSONValue *subRoot, cadf::dom::JSONBuilder &builder) {
    builder.addValue(subRoot, "val1", data.val1);
    builder.addValue(subRoot, "val2", data.val2);
}

template<>
void cadf::comms::json::loadFromBuilder<TestData>(TestData &data, const cadf::dom::JSONValue *subRoot, const cadf::dom::JSONExtractor &extractor) {
    data.val1 = extractor.getValue<int>(subRoot, "val1");
    data.val2 = extractor.getValue<double>(subRoot, "val2");
}
