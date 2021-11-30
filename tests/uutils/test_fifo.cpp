#include <gtest/gtest.h>

#include "uutils/fifo.h"
#include "uutils/fifo.hpp"

struct fifo_test_data {
};

class FifoTest
    : public testing::TestWithParam<fifo_test_data> {
};

/*INSTANTIATE_TEST_SUITE_P(FifoDataTests, FifoTest, testing::Values(
    fifo_test_data {}
));*/

TEST_F(FifoTest, integration_test) {
    fifo_t* fifo = fifo_create(16);
    EXPECT_EQ(16, fifo_size(fifo));
    EXPECT_FALSE(fifo_has_next_item(fifo));
    EXPECT_FALSE(fifo_is_full(fifo));
    EXPECT_TRUE(fifo_valid(fifo));

    EXPECT_TRUE(fifo_write_single(fifo, 42));
    EXPECT_TRUE(fifo_has_next_item(fifo));
    EXPECT_TRUE(fifo_write_single(fifo, 84));
    EXPECT_TRUE(fifo_has_next_item(fifo));

    uint8_t read_result[10];
    EXPECT_EQ(2, fifo_read(fifo, &read_result[0], 10));
    EXPECT_EQ(42, read_result[0]);
    EXPECT_EQ(84, read_result[1]);
    EXPECT_FALSE(fifo_has_next_item(fifo));

    uint8_t read_single_result;
    EXPECT_FALSE(fifo_read_single(fifo, &read_single_result));
    fifo_write_single(fifo, 15);
    EXPECT_TRUE(fifo_read_single(fifo, &read_single_result));
    EXPECT_EQ(15, read_single_result);
}

TEST_F(FifoTest, integration_test_class_interface) {
    fifo fifo(16);
    EXPECT_EQ(16, fifo.size());
    EXPECT_FALSE(fifo.has_next_item());
    EXPECT_FALSE(fifo.is_full());
    EXPECT_TRUE(fifo.valid());

    EXPECT_TRUE(fifo.write_single(42));
    EXPECT_TRUE(fifo.has_next_item());
    EXPECT_TRUE(fifo.write_single(84));
    EXPECT_TRUE(fifo.has_next_item());

    std::vector<uint8_t> expected_result = { 42, 84 };
    std::vector<uint8_t> read_result = fifo.read(10);
    EXPECT_EQ(2, read_result.size());
    EXPECT_EQ(expected_result, read_result);
    EXPECT_FALSE(fifo.has_next_item());

}
