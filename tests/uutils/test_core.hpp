#ifndef TEST_DATA_HPP_
#define TEST_DATA_HPP_

#include <gtest/gtest.h>
#include <vector>
#include "uutils/common.h"
#include "uutils/memory_debug.h"
#include "uutils/dynamic_value.h"

class CoreTest
    : public testing::Test {
    virtual void TearDown() {
        ASSERT_FALSE(memory_debug_print_report()) << "Memory leak detected.";
    }
};

template <typename T>
class CoreDataTest
    : public testing::TestWithParam<T> {
    virtual void TearDown() {
        ASSERT_FALSE(memory_debug_print_report()) << "Memory leak detected.";
    }
};

struct dynamic_test_data {
    dynamic_real real_value;
    dynamic dynamic_value;
    uint8_t dynamic_size;
};

static const auto dynamic_test_values = testing::Values(
    /* { real_value, dynamic_value, dynamic_size } */
    dynamic_test_data { 0x00, 0x00, 1 }
    ,dynamic_test_data { 0x01, 0x01, 1 }
    ,dynamic_test_data { 0x15, 0x15, 1 }
    ,dynamic_test_data { 0x3F, 0x3F, 1 }
    ,dynamic_test_data { -1, 0xFF, 1 }
    ,dynamic_test_data { -64, 0xC0, 1 }

#if DYNAMIC_HAS_2B
    ,dynamic_test_data { 0x0040, 0x4000, 2 }
    ,dynamic_test_data { 0x1595, 0x5555, 2 }
    ,dynamic_test_data { 0x203F, 0x5FFF, 2 }
    ,dynamic_test_data { -65, 0xBFFF, 2 }
    ,dynamic_test_data { (int16_t)0xEA6A, 0xAAAA, 2 }
    ,dynamic_test_data { -8256, 0xA000, 2 }
#endif

#if DYNAMIC_HAS_4B
    ,dynamic_test_data { 0x00002040, 0x60000000, 4 }
    ,dynamic_test_data { 0x0AAACAEA, 0x6AAAAAAA, 4 }
    ,dynamic_test_data { 0x1000203F, 0x6FFFFFFF, 4 }
    ,dynamic_test_data { -8257, 0x9FFFFFFF, 4 }
    ,dynamic_test_data { (int32_t)(0xF5553515), 0x95555555, 4 }
    ,dynamic_test_data { -268443712, 0x90000000, 4 }
#endif

#if DYNAMIC_HAS_8B
    ,dynamic_test_data { 0x0000000010002040, 0x7000000000000000, 8 }
    ,dynamic_test_data { 0x0555555565557595, 0x7555555555555555, 8 }
    ,dynamic_test_data { 0x080000001000203F, 0x77FFFFFFFFFFFFFF, 8 }
    ,dynamic_test_data { (int64_t)0xFFFFFFFFEFFFDFBF, 0x8FFFFFFFFFFFFFFF, 8 }
    ,dynamic_test_data { (int64_t)0xFAAAAAAA9AAA8A6A, 0x8AAAAAAAAAAAAAAA, 8 }
    ,dynamic_test_data { (int64_t)0xF7FFFFFFEFFFDFC0, 0x8800000000000000, 8 }
#endif
);

static const auto dynamic_test_positive_values = testing::Values(
    /* { real_value, dynamic_value, dynamic_size } */
    dynamic_test_data { 0x00, 0x00, 1 }
    ,dynamic_test_data { 0x01, 0x01, 1 }
    ,dynamic_test_data { 0x15, 0x15, 1 }
    ,dynamic_test_data { 0x3F, 0x3F, 1 }

#if DYNAMIC_HAS_2B
    ,dynamic_test_data { 0x0040, 0x4000, 2 }
    ,dynamic_test_data { 0x1595, 0x5555, 2 }
    ,dynamic_test_data { 0x203F, 0x5FFF, 2 }
#endif

#if DYNAMIC_HAS_4B
    ,dynamic_test_data { 0x00002040, 0x60000000, 4 }
    ,dynamic_test_data { 0x0AAACAEA, 0x6AAAAAAA, 4 }
    ,dynamic_test_data { 0x1000203F, 0x6FFFFFFF, 4 }
#endif

#if DYNAMIC_HAS_8B
    ,dynamic_test_data { 0x0000000010002040, 0x7000000000000000, 8 }
    ,dynamic_test_data { 0x0555555565557595, 0x7555555555555555, 8 }
    ,dynamic_test_data { 0x080000001000203F, 0x77FFFFFFFFFFFFFF, 8 }
#endif
);

void test_place_dynamic(dynamic value, uint8_t size, uint8_t* buffer);

struct checksum_test_data {
    std::vector<uint8_t> data;
    uint8_t result;
};

static const auto checksum_test_values = testing::Values(
    checksum_test_data { { 0 }, 0 }
    ,checksum_test_data { { 0, 1, 2, 3, 4, 5, 6, 7 }, 28 }
    ,checksum_test_data { { 1, 2, 3, 4, 5 }, 15 }
    ,checksum_test_data { { 1 }, 1 }
    ,checksum_test_data { { 255 }, 255 }
    ,checksum_test_data { { 245, 255, 134, 151 }, 17 }
    ,checksum_test_data { { 127, 128 }, 255 }
);

struct payload_test_data {
    std::vector<uint8_t> payload;
};

static const auto payload_test_values = testing::Values(
    payload_test_data { std::vector<uint8_t> { 0x00, 0x00 } }
);

#endif  // TEST_DATA_HPP_
