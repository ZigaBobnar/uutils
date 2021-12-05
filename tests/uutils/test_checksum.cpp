#include <gtest/gtest.h>
#include "uutils/checksum.h"
#include "uutils/checksum.hpp"
#include "uutils/dynamic_value.h"

struct checksum_test_data {
    std::vector<uint8_t> data;
    uint8_t result;
};

class ChecksumTest
    : public testing::TestWithParam<checksum_test_data> {
    virtual void TearDown() {
        memory_debug_print_report();
    }

protected:
    std::vector<uint8_t> vec_data;
};

INSTANTIATE_TEST_SUITE_P(ChecksumDataTests, ChecksumTest, testing::Values(
    checksum_test_data { { 0 }, 0 },
    checksum_test_data { { 0, 1, 2, 3, 4, 5, 6, 7 }, 28 },
    checksum_test_data { { 1, 2, 3, 4, 5 }, 15 },
    checksum_test_data { { 1 }, 1 },
    checksum_test_data { { 255 }, 255 },
    checksum_test_data { { 245, 255, 134, 151 }, 17 },
    checksum_test_data { { 127, 128 }, 255 }
));

struct dynamic_test_data {
    uint64_t real_value;
    uint64_t dynamic_value;
    uint8_t dynamic_size;
};

class ChecksumDynamicTest
    : public testing::TestWithParam<dynamic_test_data> {
};

INSTANTIATE_TEST_SUITE_P(ChecksumDynamicDataTest, ChecksumDynamicTest, testing::Values(
    dynamic_test_data { 0x00, 0x00, 1 },
    dynamic_test_data { 0x01, 0x01, 1 },
    dynamic_test_data { 0x40, 0x40, 1 },
    dynamic_test_data { 0x7F, 0x7F, 1 },
    dynamic_test_data { 0x0080, 0x8000, 2 },
    dynamic_test_data { 0x0100, 0x8080, 2 },
    dynamic_test_data { 0x407F, 0xBFFF, 2 },
    dynamic_test_data { 0x00004080, 0xC0000000, 4 },
    dynamic_test_data { 0x0080C100, 0xC0808080, 4 },
    dynamic_test_data { 0x2000407F, 0xDFFFFFFF, 4 }
));

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
        expected_checksum = (uint8_t)data.dynamic_value;
    } else if (data.dynamic_size == 2) {
        expected_checksum += (uint8_t)(data.dynamic_value >> 8);
        expected_checksum += (uint8_t)(data.dynamic_value);
    } else if (data.dynamic_size == 4) {
        uint32_t serialized = dynamic_serialize(data.dynamic_value, nullptr);
        expected_checksum += (uint8_t)(data.dynamic_value >> 24);
        expected_checksum += (uint8_t)(data.dynamic_value >> 16);
        expected_checksum += (uint8_t)(data.dynamic_value >> 8);
        expected_checksum += (uint8_t)(data.dynamic_value);
    }

    ASSERT_EQ(expected_checksum, checksum_simple_dynamic_value(data.real_value));
}
