#include <gtest/gtest.h>
#include "uutils/checksum.h"

class ChecksumTest : public ::testing::Test {
protected:
    std::vector<uint8_t> vec_data;


    void SetUp() override {

    }
};

TEST_F(ChecksumTest, FromVectorData) {
    vec_data = { 0, 1, 2, 3, 4, 5 };

    uint8_t result = checksum_simple(&vec_data[0], vec_data.size());
    ASSERT_EQ(1 + 2 + 3 + 4 + 5, result);

    result = checksum_simple(&vec_data[0], 3);
    ASSERT_EQ(1 + 2, result);

    result = checksum_simple(&vec_data[0], 0);
    ASSERT_EQ(0, result);
}

TEST_F(ChecksumTest, SimpleArrayChecksum) {
    uint8_t data[] = { 1, 2, 3, 4, 5, 6 };

    uint8_t result = checksum_simple(data, 6);
    ASSERT_EQ(1 + 2 + 3 + 4 + 5 + 6, result);

    result = checksum_simple(data, 3);
    ASSERT_EQ(1 + 2 + 3, result);

    result = checksum_simple(data, 0);
    ASSERT_EQ(0, result);
}

TEST_F(ChecksumTest, Overflows) {
    vec_data = {245, 255, 134, 151};

    uint8_t result = checksum_simple(&vec_data[0], vec_data.size());
    ASSERT_EQ(17, result);
}

TEST_F(ChecksumTest, CombineChecksum) {
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
