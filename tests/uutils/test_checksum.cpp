#include <gtest/gtest.h>
#include "test_core.hpp"
#include "uutils/checksum.h"
#include "uutils/checksum.hpp"
#include "uutils/dynamic_value.h"

class ChecksumTest : public CoreDataTest<checksum_test_data> {
protected:
    std::vector<uint8_t> vec_data;
};
INSTANTIATE_TEST_SUITE_P(ChecksumDataTests, ChecksumTest, checksum_test_values);

class ChecksumDynamicTest : public CoreDataTest<dynamic_test_data> {};
INSTANTIATE_TEST_SUITE_P(ChecksumDynamicDataTest, ChecksumDynamicTest, dynamic_test_values);

TEST_P(ChecksumTest, Simple_UseVectorIterators) {
    checksum_test_data data = GetParam();

    uint8_t result = checksum_simple(&data.data[0], data.data.size());
    ASSERT_EQ(data.result, result);
}

TEST_P(ChecksumTest, Simple_VectorInterface) {
    checksum_test_data data = GetParam();

    uint8_t result = checksum_simple(data.data);
    ASSERT_EQ(data.result, result);
}

TEST_F(ChecksumTest, Simple_ArrayChecksum) {
    uint8_t data[] = { 1, 2, 3, 4, 5, 6 };

    uint8_t result = checksum_simple(data, 6);
    ASSERT_EQ(1 + 2 + 3 + 4 + 5 + 6, result);

    result = checksum_simple(data, 3);
    ASSERT_EQ(1 + 2 + 3, result);

    result = checksum_simple(data, 0);
    ASSERT_EQ(0, result);
}

TEST_F(ChecksumTest, Simple_Overflows) {
    vec_data = {245, 255, 134, 151};

    uint8_t result = checksum_simple(&vec_data[0], vec_data.size());
    ASSERT_EQ(17, result);
}

TEST_F(ChecksumTest, Simple_CombineChecksum) {
    vec_data = { 0, 1, 2, 3, 4, 5 };
    uint8_t starting_checksum = 43;

    uint8_t result = checksum_simple_combine(starting_checksum, &vec_data[0], vec_data.size());
    ASSERT_EQ(1 + 2 + 3 + 4 + 5 + 43, result);

    starting_checksum = 255;
    result = checksum_simple_combine(starting_checksum, &vec_data[0], 4);
    ASSERT_EQ(5, result);

    starting_checksum = 42;
    result = checksum_simple_combine(starting_checksum, &vec_data[0], 0);
    ASSERT_EQ(42, result);
}

TEST_F(ChecksumTest, Simple_CombineChecksum_VectorInterface) {
    vec_data = { 0, 1, 2, 3, 4, 5 };
    uint8_t starting_checksum = 43;

    uint8_t result = checksum_simple_combine(starting_checksum, vec_data);
    ASSERT_EQ(1 + 2 + 3 + 4 + 5 + 43, result);
}

TEST_P(ChecksumDynamicTest, SimpleDynamicValue) {
    dynamic_test_data data = GetParam();

    uint8_t expected_checksum = 0;
    if (data.dynamic_size == 1) {
        expected_checksum += (uint8_t)data.dynamic_value;
    } else if (data.dynamic_size == 2) {
        expected_checksum += (uint8_t)(data.dynamic_value >> 8);
        expected_checksum += (uint8_t)(data.dynamic_value);
    } else if (data.dynamic_size == 4) {
        expected_checksum += (uint8_t)(data.dynamic_value >> 24);
        expected_checksum += (uint8_t)(data.dynamic_value >> 16);
        expected_checksum += (uint8_t)(data.dynamic_value >> 8);
        expected_checksum += (uint8_t)(data.dynamic_value);
    } else {
        expected_checksum += (uint8_t)(data.dynamic_value >> 56);
        expected_checksum += (uint8_t)(data.dynamic_value >> 48);
        expected_checksum += (uint8_t)(data.dynamic_value >> 40);
        expected_checksum += (uint8_t)(data.dynamic_value >> 32);
        expected_checksum += (uint8_t)(data.dynamic_value >> 24);
        expected_checksum += (uint8_t)(data.dynamic_value >> 16);
        expected_checksum += (uint8_t)(data.dynamic_value >> 8);
        expected_checksum += (uint8_t)(data.dynamic_value);
    }

    ASSERT_EQ(expected_checksum, checksum_simple_dynamic_value(data.real_value));
}
