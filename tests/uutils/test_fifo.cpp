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

TEST_F(FifoTest, IntegrationTest) {
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

TEST_F(FifoTest, IntegrationTest_ClassInterface) {
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

TEST_F(FifoTest, Create_CorrectStructure) {
    fifo_t* fifo = fifo_create(23);

    EXPECT_EQ(fifo->buffer_start, fifo->read_ptr);
    EXPECT_EQ(fifo->buffer_start, fifo->write_ptr);
    EXPECT_EQ(23, fifo->buffer_end - fifo->buffer_start);
    EXPECT_EQ(0, *fifo->buffer_start);

    fifo_destroy(&fifo);
}

TEST_F(FifoTest, ReadSingle_GetsCorrectPreSetValue) {
    fifo_t* fifo = fifo_create(10);
    *(fifo->write_ptr++) = 244;
    uint8_t value;

    EXPECT_TRUE(fifo_read_single(fifo, &value));
    EXPECT_EQ(244, value);
    EXPECT_EQ(fifo->read_ptr, fifo->write_ptr);

    fifo_destroy(&fifo);
}

TEST_F(FifoTest, ReadSingle_NoDataNoChange) {
    fifo_t* fifo = fifo_create(10);
    uint8_t value = 94;

    EXPECT_FALSE(fifo_read_single(fifo, &value));
    EXPECT_EQ(94, value);
    EXPECT_EQ(fifo->buffer_start, fifo->read_ptr);
    EXPECT_EQ(fifo->buffer_start, fifo->write_ptr);

    fifo_destroy(&fifo);
}

TEST_F(FifoTest, ReadSingle_GetsCorrectPreSetValues) {
    fifo_t* fifo = fifo_create(10);
    *(fifo->write_ptr++) = 244;
    *(fifo->write_ptr++) = 43;
    *(fifo->write_ptr++) = 94;
    uint8_t value;

    EXPECT_TRUE(fifo_read_single(fifo, &value));
    EXPECT_EQ(244, value);
    EXPECT_TRUE(fifo_read_single(fifo, &value));
    EXPECT_EQ(43, value);
    EXPECT_TRUE(fifo_read_single(fifo, &value));
    EXPECT_EQ(94, value);
    EXPECT_FALSE(fifo_read_single(fifo, &value));
    EXPECT_EQ(94, value);
    EXPECT_EQ(fifo->read_ptr, fifo->write_ptr);

    fifo_destroy(&fifo);
}

TEST_F(FifoTest, ReadSingle_TurnOver) {
    fifo_t* fifo = fifo_create(8);
    fifo->write_ptr = fifo->read_ptr = fifo->buffer_end - 1;
    *(fifo->write_ptr++) = 244;
    *(fifo->write_ptr) = 43;
    fifo->write_ptr = fifo->buffer_start;
    *(fifo->write_ptr++) = 94;
    *(fifo->write_ptr++) = 23;
    uint8_t value;

    EXPECT_TRUE(fifo_read_single(fifo, &value));
    EXPECT_EQ(244, value);
    EXPECT_TRUE(fifo_read_single(fifo, &value));
    EXPECT_EQ(43, value);
    EXPECT_TRUE(fifo_read_single(fifo, &value));
    EXPECT_EQ(94, value);
    EXPECT_TRUE(fifo_read_single(fifo, &value));
    EXPECT_EQ(23, value);
    EXPECT_FALSE(fifo_read_single(fifo, &value));
    EXPECT_EQ(23, value);
    EXPECT_EQ(fifo->read_ptr, fifo->write_ptr);

    fifo_destroy(&fifo);
}

TEST_F(FifoTest, ReadSingle_SkipOnNullBuffer) {
    fifo_t* fifo = fifo_create(10);
    *(fifo->write_ptr++) = 244;
    *(fifo->write_ptr++) = 43;

    EXPECT_TRUE(fifo_read_single(fifo, NULL));

    fifo_destroy(&fifo);
}

TEST_F(FifoTest, WriteSingle_FifoFull) {
    fifo_t* fifo = fifo_create(4);
    uint8_t* original_read = fifo->read_ptr += 2;
    uint8_t* original_write = fifo->write_ptr = fifo->read_ptr - 1;

    EXPECT_FALSE(fifo_write_single(fifo, 62));
    //TEST_ASSERT_EACH_EQUAL_INT8(0, fifo->buffer_start, 4);
    EXPECT_EQ(original_write, fifo->write_ptr);
    EXPECT_EQ(original_read, fifo->read_ptr);
    
    fifo_destroy(&fifo);
}

TEST_F(FifoTest, WriteSingle_ToStart) {
    fifo_t* fifo = fifo_create(6);
    uint8_t* original_read = fifo->read_ptr;
    uint8_t* original_write = fifo->write_ptr;

    EXPECT_TRUE(fifo_write_single(fifo, 62));
    EXPECT_EQ(original_read, fifo->read_ptr);
    EXPECT_EQ(original_write + 1, fifo->write_ptr);
    EXPECT_EQ(62, *fifo->read_ptr);
    EXPECT_EQ(0, *fifo->write_ptr);
    
    fifo_destroy(&fifo);
}

TEST_F(FifoTest, WriteSingle_ToEnd) {
    fifo_t* fifo = fifo_create(6);
    uint8_t* original_read = fifo->read_ptr = fifo->write_ptr = fifo->buffer_end;

    EXPECT_TRUE(fifo_write_single(fifo, 62));
    EXPECT_EQ(original_read, fifo->read_ptr);
    EXPECT_EQ(fifo->buffer_start, fifo->write_ptr);
    EXPECT_EQ(62, *fifo->read_ptr);
    EXPECT_EQ(0, *fifo->write_ptr);
    
    fifo_destroy(&fifo);
}

TEST_F(FifoTest, WriteSingle_MultipleInMiddle) {
    fifo_t* fifo = fifo_create(12);
    uint8_t* original_read = fifo->read_ptr += 3;
    uint8_t* original_write = fifo->write_ptr = original_read;

    EXPECT_TRUE(fifo_write_single(fifo, 62));
    EXPECT_EQ(original_read, fifo->read_ptr);
    EXPECT_EQ(original_write + 1, fifo->write_ptr);
    EXPECT_EQ(62, *fifo->read_ptr);
    EXPECT_EQ(0, *fifo->write_ptr);

    EXPECT_TRUE(fifo_write_single(fifo, 21));
    EXPECT_EQ(original_read, fifo->read_ptr);
    EXPECT_EQ(original_write + 2, fifo->write_ptr);
    EXPECT_EQ(62, *fifo->read_ptr);
    EXPECT_EQ(21, *(fifo->read_ptr + 1));
    EXPECT_EQ(0, *fifo->write_ptr);
    
    fifo_destroy(&fifo);
}

TEST_F(FifoTest, WriteSingle_MultipleOnEndRollover) {
    fifo_t* fifo = fifo_create(12);
    uint8_t* original_read = fifo->read_ptr = fifo->write_ptr = fifo->buffer_end - 1;

    EXPECT_TRUE(fifo_write_single(fifo, 62));
    EXPECT_EQ(original_read, fifo->read_ptr);
    EXPECT_EQ(fifo->buffer_end, fifo->write_ptr);
    EXPECT_EQ(62, *fifo->read_ptr);
    EXPECT_EQ(0, *fifo->write_ptr);

    EXPECT_TRUE(fifo_write_single(fifo, 21));
    EXPECT_EQ(original_read, fifo->read_ptr);
    EXPECT_EQ(fifo->buffer_start, fifo->write_ptr);
    EXPECT_EQ(62, *fifo->read_ptr);
    EXPECT_EQ(21, *(fifo->read_ptr + 1));
    EXPECT_EQ(0, *fifo->write_ptr);
    
    EXPECT_TRUE(fifo_write_single(fifo, 94));
    EXPECT_EQ(original_read, fifo->read_ptr);
    EXPECT_EQ(fifo->buffer_start + 1, fifo->write_ptr);
    EXPECT_EQ(62, *fifo->read_ptr);
    EXPECT_EQ(21, *(fifo->read_ptr + 1));
    EXPECT_EQ(94, *(fifo->write_ptr - 1));
    EXPECT_EQ(0, *fifo->write_ptr);
    
    fifo_destroy(&fifo);
}

TEST_F(FifoTest, WriteSingle_OverflowSmallBufferReturnsFalse) {
    fifo_t* fifo = fifo_create(4);
    
    EXPECT_TRUE(fifo_write_single(fifo, 62));
    EXPECT_TRUE(fifo_write_single(fifo, 35));
    EXPECT_TRUE(fifo_write_single(fifo, 15));
    EXPECT_TRUE(fifo_write_single(fifo, 46));
    EXPECT_FALSE(fifo_write_single(fifo, 46));
    
    fifo_destroy(&fifo);
}

TEST_F(FifoTest, Valid_CheckValidManual) {
    uint8_t* data_buffer = (uint8_t*)malloc(sizeof(uint8_t) * 17);

    fifo_t fifo = {
        /* read_ptr */ data_buffer,
        /* write_ptr */ data_buffer,
        /* buffer_start */ data_buffer,
        /* buffer_end */ data_buffer + 16,
    };

    EXPECT_TRUE(fifo_valid(&fifo));

    free(data_buffer);
}

TEST_F(FifoTest, Valid_CheckValidNew) {
    fifo_t* fifo = fifo_create(16);

    EXPECT_TRUE(fifo_valid(fifo));

    fifo_destroy(&fifo);
}

TEST_F(FifoTest, Valid_InvalidAfterDestroy) {
    fifo_t* fifo = fifo_create(16);
    fifo_destroy(&fifo);

    EXPECT_FALSE(fifo_valid(fifo));
}

TEST_F(FifoTest, Valid_InvalidWritePtr) {
    fifo_t* fifo = fifo_create(16);

    fifo->write_ptr = fifo->buffer_end + 1;

    EXPECT_FALSE(fifo_valid(fifo));

    fifo_destroy(&fifo);
}

TEST_F(FifoTest, Valid_InvalidWritePtr2) {
    fifo_t* fifo = fifo_create(16);

    fifo->write_ptr = fifo->buffer_start - 1;

    EXPECT_FALSE(fifo_valid(fifo));

    fifo_destroy(&fifo);
}

TEST_F(FifoTest, Valid_InvalidReadPtr) {
    fifo_t* fifo = fifo_create(16);

    fifo->read_ptr = fifo->buffer_end + 1;

    EXPECT_FALSE(fifo_valid(fifo));

    fifo_destroy(&fifo);
}

TEST_F(FifoTest, Valid_InvalidReadPtr2) {
    fifo_t* fifo = fifo_create(16);

    fifo->write_ptr = fifo->buffer_start - 1;

    EXPECT_FALSE(fifo_valid(fifo));

    fifo_destroy(&fifo);
}

TEST_F(FifoTest, Reset_ResetsToBufferBegin) {
    fifo_t* fifo = fifo_create(16);
    fifo_write_single(fifo, 23);
    fifo_write_single(fifo, 22);
    fifo_read_single(fifo, NULL);

    EXPECT_EQ(fifo->buffer_start + 2, fifo->write_ptr);
    EXPECT_EQ(fifo->buffer_start + 1, fifo->read_ptr);

    fifo_reset(fifo);

    EXPECT_EQ(fifo->buffer_start, fifo->write_ptr);
    EXPECT_EQ(fifo->buffer_start, fifo->read_ptr);

    fifo_destroy(&fifo);
}

TEST_F(FifoTest, Size_SameValueAsCreate) {
    fifo_t* fifo = fifo_create(16);

    EXPECT_EQ(16, fifo_size(fifo));

    fifo_destroy(&fifo);
}

TEST_F(FifoTest, PeekSingle_PeeksWithoutIncrementingReadPtr) {
    fifo_t* fifo = fifo_create(16);
    fifo_write_single(fifo, 24);
    uint8_t* original_read = fifo->read_ptr;
    uint8_t* original_write = fifo->write_ptr;
    uint8_t value;

    EXPECT_TRUE(fifo_peek_single(fifo, &value));
    EXPECT_EQ(24, value);
    EXPECT_EQ(original_read, fifo->read_ptr);
    EXPECT_EQ(original_write, fifo->write_ptr);

    fifo_destroy(&fifo);
}

TEST_F(FifoTest, PeekSingle_PeeksFalseWhenNoData) {
    fifo_t* fifo = fifo_create(16);
    uint8_t* original_read = fifo->read_ptr;
    uint8_t* original_write = fifo->write_ptr;
    uint8_t value = 32;

    EXPECT_FALSE(fifo_peek_single(fifo, &value));
    EXPECT_EQ(32, value);
    EXPECT_EQ(original_read, fifo->read_ptr);
    EXPECT_EQ(original_write, fifo->write_ptr);

    fifo_destroy(&fifo);
}

TEST_F(FifoTest, PeekSingle_PeekMultipleTimesEachTimeSame) {
    fifo_t* fifo = fifo_create(16);
    fifo_write_single(fifo, 24);
    fifo_write_single(fifo, 93);
    fifo_write_single(fifo, 56);
    uint8_t* original_read = fifo->read_ptr;
    uint8_t* original_write = fifo->write_ptr;
    uint8_t value;

    EXPECT_TRUE(fifo_peek_single(fifo, &value));
    EXPECT_EQ(24, value);
    EXPECT_EQ(original_read, fifo->read_ptr);
    EXPECT_EQ(original_write, fifo->write_ptr);
    EXPECT_TRUE(fifo_peek_single(fifo, &value));
    EXPECT_EQ(24, value);
    EXPECT_EQ(original_read, fifo->read_ptr);
    EXPECT_EQ(original_write, fifo->write_ptr);
    EXPECT_TRUE(fifo_peek_single(fifo, &value));
    EXPECT_EQ(24, value);
    EXPECT_EQ(original_read, fifo->read_ptr);
    EXPECT_EQ(original_write, fifo->write_ptr);

    fifo_destroy(&fifo);
}

TEST_F(FifoTest, PeekSingle_PeekMultipleTimesReadMovingForward) {
    fifo_t* fifo = fifo_create(16);
    fifo_write_single(fifo, 24);
    fifo_write_single(fifo, 93);
    fifo_write_single(fifo, 56);
    uint8_t* original_read = fifo->read_ptr;
    uint8_t* original_write = fifo->write_ptr;
    uint8_t value;

    EXPECT_TRUE(fifo_peek_single(fifo, &value));
    EXPECT_EQ(24, value);
    EXPECT_EQ(original_read, fifo->read_ptr);
    EXPECT_EQ(original_write, fifo->write_ptr);
    EXPECT_TRUE(fifo_read_single(fifo, NULL));
    EXPECT_TRUE(fifo_peek_single(fifo, &value));
    EXPECT_EQ(93, value);
    EXPECT_EQ(original_read + 1, fifo->read_ptr);
    EXPECT_EQ(original_write, fifo->write_ptr);
    EXPECT_TRUE(fifo_read_single(fifo, NULL));
    EXPECT_TRUE(fifo_peek_single(fifo, &value));
    EXPECT_EQ(56, value);
    EXPECT_EQ(original_read + 2, fifo->read_ptr);
    EXPECT_EQ(original_write, fifo->write_ptr);

    fifo_destroy(&fifo);
}

TEST_F(FifoTest, HasNextItem_ReturnsTrueWhenAvailable) {
    fifo_t* fifo = fifo_create(16);
    fifo_write_single(fifo, 13);

    EXPECT_TRUE(fifo_has_next_item(fifo));

    fifo_destroy(&fifo);
}

TEST_F(FifoTest, HasNextItem_ReturnsFalseWhenNotAvailable) {
    fifo_t* fifo = fifo_create(16);
    
    EXPECT_FALSE(fifo_has_next_item(fifo));

    fifo_destroy(&fifo);
}

TEST_F(FifoTest, HasNextItem_ReturnsFalseWhenAlreadyRead) {
    fifo_t* fifo = fifo_create(16);
    fifo_write_single(fifo, 24);
    fifo_read_single(fifo, NULL);
    
    EXPECT_FALSE(fifo_has_next_item(fifo));

    fifo_destroy(&fifo);
}

TEST_F(FifoTest, IsFull_ReturnsFalseOnEmpty) {
    fifo_t* fifo = fifo_create(16);

    EXPECT_FALSE(fifo_is_full(fifo));

    fifo_destroy(&fifo);
}

TEST_F(FifoTest, IsFull_SmallFifoOverflow) {
    fifo_t* fifo = fifo_create(3);
    
    EXPECT_TRUE(fifo_write_single(fifo, 24));
    EXPECT_TRUE(fifo_write_single(fifo, 35));
    EXPECT_TRUE(fifo_write_single(fifo, 45));
    EXPECT_TRUE(fifo_is_full(fifo));

    fifo_destroy(&fifo);
}

TEST_F(FifoTest, IsFull_SmallFifoOverflow2) {
    fifo_t* fifo = fifo_create(3);
    
    EXPECT_TRUE(fifo_write_single(fifo, 24));
    EXPECT_TRUE(fifo_write_single(fifo, 35));
    EXPECT_TRUE(fifo_read_single(fifo, NULL));
    EXPECT_TRUE(fifo_write_single(fifo, 45));
    EXPECT_TRUE(fifo_write_single(fifo, 235));
    EXPECT_TRUE(fifo_is_full(fifo));

    fifo_destroy(&fifo);
}

TEST_F(FifoTest, Read_ReadsNothingWhenNoData) {
    fifo_t* fifo = fifo_create(16);
    uint8_t buf[17] = { 0 };
    
    EXPECT_EQ(0, fifo_read(fifo, buf, 16));
    EXPECT_EQ(fifo->buffer_start, fifo->write_ptr);
    EXPECT_EQ(fifo->buffer_start, fifo->read_ptr);
    //TEST_ASSERT_EACH_EQUAL_INT8(0, buf, 16);

    fifo_destroy(&fifo);
}

TEST_F(FifoTest, Read_ReadsIntoBuffer) {
    fifo_t* fifo = fifo_create(16);
    fifo_write_single(fifo, 24);
    fifo_write_single(fifo, 63);
    fifo_write_single(fifo, 37);
    uint8_t buf[17] = { 0 };
    
    EXPECT_EQ(3, fifo_read(fifo, buf, 16));
    EXPECT_EQ(fifo->buffer_start + 3, fifo->write_ptr);
    EXPECT_EQ(fifo->buffer_start + 3, fifo->read_ptr);
    EXPECT_EQ(24, buf[0]);
    EXPECT_EQ(63, buf[1]);
    EXPECT_EQ(37, buf[2]);
    //TEST_ASSERT_EACH_EQUAL_INT8(0, buf + 3, 16 - 3);

    fifo_destroy(&fifo);
}

TEST_F(FifoTest, Read_ReadNothingLimitedByArg) {
    fifo_t* fifo = fifo_create(16);
    fifo_write_single(fifo, 24);
    fifo_write_single(fifo, 63);
    uint8_t buf[17] = { 0 };
    
    EXPECT_EQ(0, fifo_read(fifo, buf, 0));
    EXPECT_EQ(fifo->buffer_start + 2, fifo->write_ptr);
    EXPECT_EQ(fifo->buffer_start, fifo->read_ptr);
    //TEST_ASSERT_EACH_EQUAL_INT8(0, buf, 16);

    fifo_destroy(&fifo);
}

TEST_F(FifoTest, Read_ReadLimitedByArg) {
    fifo_t* fifo = fifo_create(16);
    fifo_write_single(fifo, 24);
    fifo_write_single(fifo, 63);
    fifo_write_single(fifo, 37);
    fifo_write_single(fifo, 35);
    fifo_write_single(fifo, 75);
    uint8_t buf[17] = { 0 };
    
    EXPECT_EQ(4, fifo_read(fifo, buf, 4));
    EXPECT_EQ(fifo->buffer_start + 5, fifo->write_ptr);
    EXPECT_EQ(fifo->buffer_start + 4, fifo->read_ptr);
    EXPECT_EQ(24, buf[0]);
    EXPECT_EQ(63, buf[1]);
    EXPECT_EQ(37, buf[2]);
    EXPECT_EQ(35, buf[3]);
    //TEST_ASSERT_EACH_EQUAL_INT8(0, buf + 4, 16 - 4);

    fifo_destroy(&fifo);
}

TEST_F(FifoTest, Read_ReadFull) {
    fifo_t* fifo = fifo_create(4);
    fifo_write_single(fifo, 24);
    fifo_write_single(fifo, 63);
    fifo_write_single(fifo, 37);
    fifo_write_single(fifo, 35);
    uint8_t buf[5] = { 0 };
    
    EXPECT_EQ(4, fifo_read(fifo, buf, 16));
    EXPECT_EQ(fifo->buffer_end, fifo->write_ptr);
    EXPECT_EQ(fifo->buffer_end, fifo->read_ptr);
    EXPECT_EQ(24, buf[0]);
    EXPECT_EQ(63, buf[1]);
    EXPECT_EQ(37, buf[2]);
    EXPECT_EQ(35, buf[3]);

    fifo_destroy(&fifo);
}

TEST_F(FifoTest, Peek_ReadsNothingWhenNoData) {
    fifo_t* fifo = fifo_create(16);
    uint8_t buf[17] = { 0 };
    
    EXPECT_EQ(0, fifo_peek(fifo, buf, 16));
    EXPECT_EQ(fifo->buffer_start, fifo->write_ptr);
    EXPECT_EQ(fifo->buffer_start, fifo->read_ptr);
    //TEST_ASSERT_EACH_EQUAL_INT8(0, buf, 16);

    fifo_destroy(&fifo);
}

TEST_F(FifoTest, Peek_ReadsIntoBuffer) {
    fifo_t* fifo = fifo_create(16);
    fifo_write_single(fifo, 24);
    fifo_write_single(fifo, 63);
    fifo_write_single(fifo, 37);
    uint8_t buf[17] = { 0 };
    
    EXPECT_EQ(3, fifo_peek(fifo, buf, 16));
    EXPECT_EQ(fifo->buffer_start + 3, fifo->write_ptr);
    EXPECT_EQ(fifo->buffer_start, fifo->read_ptr);
    EXPECT_EQ(24, buf[0]);
    EXPECT_EQ(63, buf[1]);
    EXPECT_EQ(37, buf[2]);
    //TEST_ASSERT_EACH_EQUAL_INT8(0, buf + 3, 16 - 3);

    fifo_destroy(&fifo);
}

TEST_F(FifoTest, Peek_ReadNothingLimitedByArg) {
    fifo_t* fifo = fifo_create(16);
    fifo_write_single(fifo, 24);
    fifo_write_single(fifo, 63);
    uint8_t buf[17] = { 0 };
    
    EXPECT_EQ(0, fifo_peek(fifo, buf, 0));
    EXPECT_EQ(fifo->buffer_start + 2, fifo->write_ptr);
    EXPECT_EQ(fifo->buffer_start, fifo->read_ptr);
    //TEST_ASSERT_EACH_EQUAL_INT8(0, buf, 16);

    fifo_destroy(&fifo);
}

TEST_F(FifoTest, Peek_ReadLimitedByArg) {
    fifo_t* fifo = fifo_create(16);
    fifo_write_single(fifo, 24);
    fifo_write_single(fifo, 63);
    fifo_write_single(fifo, 37);
    fifo_write_single(fifo, 35);
    fifo_write_single(fifo, 75);
    uint8_t buf[17] = { 0 };
    
    EXPECT_EQ(4, fifo_peek(fifo, buf, 4));
    EXPECT_EQ(fifo->buffer_start + 5, fifo->write_ptr);
    EXPECT_EQ(fifo->buffer_start, fifo->read_ptr);
    EXPECT_EQ(24, buf[0]);
    EXPECT_EQ(63, buf[1]);
    EXPECT_EQ(37, buf[2]);
    EXPECT_EQ(35, buf[3]);
    //TEST_ASSERT_EACH_EQUAL_INT8(0, buf + 4, 16 - 4);

    fifo_destroy(&fifo);
}

TEST_F(FifoTest, Peek_ReadFull) {
    fifo_t* fifo = fifo_create(4);
    fifo_write_single(fifo, 24);
    fifo_write_single(fifo, 63);
    fifo_write_single(fifo, 37);
    fifo_write_single(fifo, 35);
    uint8_t buf[5] = { 0 };
    
    EXPECT_EQ(4, fifo_peek(fifo, buf, 16));
    EXPECT_EQ(fifo->buffer_end, fifo->write_ptr);
    EXPECT_EQ(fifo->buffer_start, fifo->read_ptr);
    EXPECT_EQ(24, buf[0]);
    EXPECT_EQ(63, buf[1]);
    EXPECT_EQ(37, buf[2]);
    EXPECT_EQ(35, buf[3]);

    fifo_destroy(&fifo);
}

TEST_F(FifoTest, Write_NoDataToWrite) {
    fifo_t* fifo = fifo_create(16);
    uint8_t buf[5] = { 0 };
    
    EXPECT_EQ(0, fifo_write(fifo, buf, 0));
    EXPECT_EQ(fifo->buffer_start, fifo->write_ptr);
    EXPECT_EQ(fifo->buffer_start, fifo->read_ptr);

    fifo_destroy(&fifo);
}

TEST_F(FifoTest, Write_WritesAllData) {
    fifo_t* fifo = fifo_create(16);
    uint8_t buf[5] = { 34, 45, 46, 53 };
    
    EXPECT_EQ(4, fifo_write(fifo, buf, 4));
    EXPECT_EQ(fifo->buffer_start + 4, fifo->write_ptr);
    EXPECT_EQ(fifo->buffer_start, fifo->read_ptr);
    EXPECT_EQ(34, fifo->buffer_start[0]);
    EXPECT_EQ(45, fifo->buffer_start[1]);
    EXPECT_EQ(46, fifo->buffer_start[2]);
    EXPECT_EQ(53, fifo->buffer_start[3]);

    fifo_destroy(&fifo);
}

TEST_F(FifoTest, Write_SmallBufferWriteUntilFull) {
    fifo_t* fifo = fifo_create(4);
    uint8_t buf[9] = { 34, 45, 46, 53, 57, 24, 41, 78 };
    
    EXPECT_EQ(4, fifo_write(fifo, buf, 9));
    EXPECT_EQ(fifo->buffer_end, fifo->write_ptr);
    EXPECT_EQ(fifo->buffer_start, fifo->read_ptr);
    EXPECT_EQ(34, fifo->buffer_start[0]);
    EXPECT_EQ(45, fifo->buffer_start[1]);
    EXPECT_EQ(46, fifo->buffer_start[2]);
    EXPECT_EQ(53, fifo->buffer_start[3]);

    fifo_destroy(&fifo);
}
