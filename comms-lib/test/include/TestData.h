#ifndef INCLUDE_TESTDATA_H_
#define INCLUDE_TESTDATA_H_

#include <iostream>

/**
 * Data struct to be used for testing
 */
struct TestData {
    int val1;
    double val2;
};

bool operator==(const TestData& lhs, const TestData& rhs);

std::ostream& operator<<(std::ostream& stream, const TestData& data);

#endif /* INCLUDE_TESTDATA_H_ */
