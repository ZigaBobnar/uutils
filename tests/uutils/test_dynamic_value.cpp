#include <gtest/gtest.h>
#include "test_core.hpp"
#include "uutils/dynamic_value.h"
#include "uutils/dynamic_value.hpp"

class DynamicValueTest : public CoreDataTest<dynamic_test_data> {};
INSTANTIATE_TEST_SUITE_P(DynamicValueDataTests, DynamicValueTest, dynamic_test_values);

TEST_P(DynamicValueTest, ToReal_DirectInput) {
    dynamic_test_data data = GetParam();

    uint8_t dynamic_size;
    EXPECT_EQ(data.real_value, dynamic_to_real(data.dynamic_value, &dynamic_size));
    EXPECT_EQ(data.dynamic_size, dynamic_size);
}

TEST_P(DynamicValueTest, ToReal_NoBufferReference) {
    dynamic_test_data data = GetParam();

    EXPECT_EQ(data.real_value, dynamic_to_real(data.dynamic_value, nullptr));
}

TEST_P(DynamicValueTest, BufferToReal) {
    dynamic_test_data data = GetParam();
    uint8_t buffer[9];
    uint8_t* buffer_pos = nullptr;
    test_place_dynamic(data.dynamic_value, data.dynamic_size, buffer);

    EXPECT_EQ(data.real_value, dynamic_buffer_to_real(buffer, &buffer_pos));
    EXPECT_EQ(data.dynamic_size, buffer_pos - buffer + 1);
}

TEST_P(DynamicValueTest, ToReal_RequiredBytes) {
    dynamic_test_data data = GetParam();
    uint8_t buffer[9];
    test_place_dynamic(data.dynamic_value, data.dynamic_size, buffer);

    EXPECT_EQ(data.dynamic_size, dynamic_to_real_get_required_bytes(buffer[0]));
}

TEST_P(DynamicValueTest, ToDynamic) {
    dynamic_test_data data = GetParam();

    uint8_t written_bytes;
    EXPECT_EQ(data.dynamic_value, real_to_dynamic(data.real_value, &written_bytes));
    EXPECT_EQ(data.dynamic_size, written_bytes);
}

TEST_P(DynamicValueTest, ToDynamic_RequiredBytes) {
    dynamic_test_data data = GetParam();

    EXPECT_EQ(data.dynamic_size, real_to_dynamic_get_required_bytes(data.real_value));
}

TEST_P(DynamicValueTest, IntegrationTest) {
    dynamic_test_data data = GetParam();
    uint8_t buffer[9];
    test_place_dynamic(data.dynamic_value, data.dynamic_size, buffer);

    uint8_t* parsed_ptr;
    dynamic_real real_value = dynamic_buffer_to_real(buffer, &parsed_ptr);

    uint8_t serialized_size;
    dynamic serialized_dynamic = real_to_dynamic(real_value, &serialized_size);
    EXPECT_EQ(data.dynamic_size, serialized_size);
    EXPECT_EQ(data.dynamic_value, serialized_dynamic);

    dynamic_real reparsed_value = dynamic_to_real(serialized_dynamic, &serialized_size);
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

    std::tuple<dynamic_real, uint8_t> parsed_value = dynamic_to_real(input_data);
    EXPECT_EQ(data.real_value, std::get<0>(parsed_value));
    EXPECT_EQ(data.dynamic_size, std::get<1>(parsed_value));

    std::vector<uint8_t> serialized = real_to_dynamic_vector(std::get<0>(parsed_value));
    EXPECT_EQ(input_data, serialized);
    EXPECT_EQ(data.dynamic_size, serialized.size());

    std::tuple<dynamic_real, uint8_t> reparsed_value = dynamic_to_real(serialized);
    EXPECT_EQ(data.real_value, std::get<0>(reparsed_value));
    EXPECT_EQ(data.dynamic_size, std::get<1>(reparsed_value));
}
