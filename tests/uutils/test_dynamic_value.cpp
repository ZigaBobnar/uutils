#include <gtest/gtest.h>
#include "uutils/dynamic_value.h"
#include "uutils/dynamic_value.hpp"

struct dynamic_test_data {
    uint64_t real_value;
    uint64_t dynamic_value;
    uint8_t dynamic_size;
};

void test_place_dynamic(uint64_t value, uint8_t size, uint8_t* buffer) {
    if (size == 4) {
        buffer[0] = (uint8_t)(value >> 24);
        buffer[1] = (uint8_t)(value >> 16);
        buffer[2] = (uint8_t)(value >> 8);
        buffer[3] = (uint8_t)(value);
    } else if (size == 2) {
        buffer[0] = (uint8_t)(value >> 8);
        buffer[1] = (uint8_t)(value);
    } else if (size == 1) {
        buffer[0] = (uint8_t)(value);
    } else {
        EXPECT_TRUE(false);
    }
}

class DynamicValueTest
    : public testing::TestWithParam<dynamic_test_data> {
    virtual void TearDown() {
        memory_debug_print_report();
    }
};

INSTANTIATE_TEST_SUITE_P(DynamicValueDataTests, DynamicValueTest, testing::Values(
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

TEST_P(DynamicValueTest, Parse_DirectInput) {
    dynamic_test_data data = GetParam();

    uint8_t dynamic_size;
    EXPECT_EQ(data.real_value, dynamic_parse(data.dynamic_value, &dynamic_size));
    EXPECT_EQ(data.dynamic_size, dynamic_size);
}

TEST_P(DynamicValueTest, Parse_Buffer) {
    dynamic_test_data data = GetParam();
    uint8_t buffer[9];
    uint8_t* buffer_pos = nullptr;
    test_place_dynamic(data.dynamic_value, data.dynamic_size, buffer);

    EXPECT_EQ(data.real_value, dynamic_parse_buffer(buffer, &buffer_pos));
    EXPECT_EQ(data.dynamic_size, buffer_pos - buffer + 1);
}

TEST_P(DynamicValueTest, Parse_NoBufferReference) {
    dynamic_test_data data = GetParam();

    EXPECT_EQ(data.real_value, dynamic_parse(data.dynamic_value, nullptr));
}

TEST_P(DynamicValueTest, Parse_RequiredBytes) {
    dynamic_test_data data = GetParam();
    uint8_t buffer[9];
    test_place_dynamic(data.dynamic_value, data.dynamic_size, buffer);

    EXPECT_EQ(data.dynamic_size, dynamic_parse_get_required_bytes(buffer[0]));
}

TEST_P(DynamicValueTest, Serialize) {
    dynamic_test_data data = GetParam();

    uint8_t written_bytes;
    EXPECT_EQ(data.dynamic_value, dynamic_serialize(data.real_value, &written_bytes));
    EXPECT_EQ(data.dynamic_size, written_bytes);
}

TEST_P(DynamicValueTest, SerializeRequiredBytes) {
    dynamic_test_data data = GetParam();

    EXPECT_EQ(data.dynamic_size, dynamic_serialize_get_required_bytes(data.real_value));
}

TEST_P(DynamicValueTest, IntegrationTest) {
    dynamic_test_data data = GetParam();
    uint8_t buffer[9];
    test_place_dynamic(data.dynamic_value, data.dynamic_size, buffer);

    uint8_t* parsed_ptr;
    uint64_t real_value = dynamic_parse_buffer(buffer, &parsed_ptr);

    uint8_t serialized_size;
    uint64_t serialized_dynamic = dynamic_serialize(real_value, &serialized_size);
    EXPECT_EQ(data.dynamic_size, serialized_size);
    EXPECT_EQ(data.dynamic_value, serialized_dynamic);

    uint64_t reparsed_value = dynamic_parse(serialized_dynamic, &serialized_size);
    EXPECT_EQ(data.real_value, reparsed_value);
    EXPECT_EQ(data.dynamic_value, serialized_dynamic);
}

TEST_P(DynamicValueTest, SplitToBytes) {
    dynamic_test_data data = GetParam();

    std::vector<uint8_t> result = dynamic_split_to_bytes(data.dynamic_value);
    EXPECT_EQ(data.dynamic_size, result.size());
}

TEST_P(DynamicValueTest, IntegrationTest_VectorInterface) {
    dynamic_test_data data = GetParam();

    std::vector<uint8_t> input_data = dynamic_split_to_bytes(data.dynamic_value);
    EXPECT_EQ(data.dynamic_size, input_data.size());

    std::tuple<uint64_t, uint8_t> parsed_value = dynamic_parse(input_data);
    EXPECT_EQ(data.real_value, std::get<0>(parsed_value));
    EXPECT_EQ(data.dynamic_size, std::get<1>(parsed_value));

    std::vector<uint8_t> serialized = dynamic_serialize_to_vector(std::get<0>(parsed_value));
    EXPECT_EQ(input_data, serialized);
    EXPECT_EQ(data.dynamic_size, serialized.size());

    std::tuple<uint64_t, uint8_t> reparsed_value = dynamic_parse(serialized);
    EXPECT_EQ(data.real_value, std::get<0>(reparsed_value));
    EXPECT_EQ(data.dynamic_size, std::get<1>(reparsed_value));
}
