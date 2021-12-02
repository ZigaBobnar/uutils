#include <gtest/gtest.h>
#include "uutils/uproto/uproto.h"
#include "uutils/dynamic_value.hpp"

class UprotoParserTests : public testing::Test {
};

struct dynamic_test_data {
    uint64_t real_value;
    uint64_t dynamic_value;
    uint8_t dynamic_size;
};

class UprotoParserDynamicValueTests : public UprotoParserTests,
    public testing::WithParamInterface<dynamic_test_data> {
};

INSTANTIATE_TEST_SUITE_P(DynamicParserTests, UprotoParserDynamicValueTests, testing::Values(
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

struct payload_test_data {
    std::vector<uint8_t> payload;
};

class UprotoParserPayloadTests : public UprotoParserTests,
    public testing::WithParamInterface<payload_test_data> {
};

INSTANTIATE_TEST_SUITE_P(PayloadParserTests, UprotoParserPayloadTests, testing::Values(
    payload_test_data { std::vector<uint8_t> { 0x00, 0x00 } }
));

TEST_F(UprotoParserTests, Initialization) {
    uproto_parser_runtime_t* runtime = uproto_parser_create();

    EXPECT_EQ(uproto_message_parser_state_message_start, runtime->state);
    EXPECT_EQ(NULL, runtime->parsing_message);
    EXPECT_EQ(NULL, runtime->ready_message);

    uproto_parser_destroy(&runtime);

    EXPECT_EQ(NULL, runtime);
}

TEST_F(UprotoParserTests, ParseSingle_ZeroData_NoParsing) {
    uproto_parser_runtime_t* runtime = uproto_parser_create();

    for (size_t i = 0; i < 50; i++) {
        EXPECT_EQ(uproto_message_parser_result_ok, uproto_parser_parse_single(runtime, 0x00));
        EXPECT_TRUE(runtime->parsing_message == NULL);
        EXPECT_FALSE(uproto_parser_has_message_ready(runtime));
        ASSERT_EQ(uproto_message_parser_state_message_start, runtime->state);
    }

    uproto_parser_destroy(&runtime);
}

TEST_F(UprotoParserTests, ParseSingle_MessageStart) {
    uproto_parser_runtime_t* runtime = uproto_parser_create();

    EXPECT_EQ(uproto_message_parser_result_ok, uproto_parser_parse_single(runtime, uproto_message_start));
    EXPECT_EQ(uproto_message_parser_state_message_params, runtime->state);
    ASSERT_FALSE(runtime->parsing_message == NULL);
    EXPECT_EQ(uproto_message_status_parsing, runtime->parsing_message->parse_status);
    EXPECT_FALSE(uproto_parser_has_message_ready(runtime));

    uproto_parser_destroy(&runtime);
}

TEST_F(UprotoParserTests, ParseSingle_MessageStart_InvalidStart) {
    uproto_parser_runtime_t* runtime = uproto_parser_create();

    EXPECT_EQ(uproto_message_parser_result_ok, uproto_parser_parse_single(runtime, 0x43));
    EXPECT_EQ(uproto_message_parser_state_message_start, runtime->state);
    EXPECT_TRUE(runtime->parsing_message == NULL);
    EXPECT_FALSE(uproto_parser_has_message_ready(runtime));

    uproto_parser_destroy(&runtime);
}

TEST_F(UprotoParserTests, ParseSingle_MessageStart_DelayedStart) {
    uproto_parser_runtime_t* runtime = uproto_parser_create();

    for (size_t i = 0; i < 50; i++) {
        EXPECT_EQ(uproto_message_parser_result_ok, uproto_parser_parse_single(runtime, 0x00));
        EXPECT_TRUE(runtime->parsing_message == NULL);
        EXPECT_FALSE(uproto_parser_has_message_ready(runtime));
        ASSERT_EQ(uproto_message_parser_state_message_start, runtime->state);
    }

    EXPECT_EQ(uproto_message_parser_result_ok, uproto_parser_parse_single(runtime, uproto_message_start));
    EXPECT_EQ(uproto_message_parser_state_message_params, runtime->state);
    ASSERT_FALSE(runtime->parsing_message == NULL);
    EXPECT_EQ(uproto_message_status_parsing, runtime->parsing_message->parse_status);
    EXPECT_FALSE(uproto_parser_has_message_ready(runtime));

    uproto_parser_destroy(&runtime);
}

TEST_F(UprotoParserTests, ParseSingle_MessageParams_RequestMessage) {
    uproto_parser_runtime_t* runtime = uproto_parser_create();
    uproto_parser_parse_single(runtime, uproto_message_start);

    EXPECT_EQ(uproto_message_parser_result_ok, uproto_parser_parse_single(runtime, 0b00000000));
    EXPECT_EQ(uproto_message_parser_state_resource_id, runtime->state);
    ASSERT_FALSE(runtime->parsing_message == NULL);
    EXPECT_EQ(uproto_message_status_parsing, runtime->parsing_message->parse_status);
    EXPECT_FALSE(uproto_parser_has_message_ready(runtime));
    EXPECT_EQ(0x00, runtime->parsing_message->message_properties);

    uproto_parser_destroy(&runtime);
}

TEST_F(UprotoParserTests, ParseSingle_MessageParams_ResponseMessage) {
    uproto_parser_runtime_t* runtime = uproto_parser_create();
    uproto_parser_parse_single(runtime, uproto_message_start);

    EXPECT_EQ(uproto_message_parser_result_ok, uproto_parser_parse_single(runtime, 0b00000001));
    EXPECT_EQ(uproto_message_parser_state_resource_id, runtime->state);
    ASSERT_FALSE(runtime->parsing_message == NULL);
    EXPECT_EQ(uproto_message_status_parsing, runtime->parsing_message->parse_status);
    EXPECT_FALSE(uproto_parser_has_message_ready(runtime));
    EXPECT_EQ(0x01, runtime->parsing_message->message_properties);

    uproto_parser_destroy(&runtime);
}

TEST_P(UprotoParserDynamicValueTests, ParseSingle_ResourceId) {
    dynamic_test_data data = GetParam();
    uproto_parser_runtime_t* runtime = uproto_parser_create();
    uproto_parser_parse_single(runtime, uproto_message_start);
    uproto_parser_parse_single(runtime, 0b00000001);

    std::vector<uint8_t> dynamic_buffer = dynamic_split_to_bytes(data.dynamic_value);

    for (const auto& input : dynamic_buffer) {
        EXPECT_EQ(uproto_message_parser_state_resource_id, runtime->state);

        EXPECT_EQ(uproto_message_parser_result_ok, uproto_parser_parse_single(runtime, input));
    }

    EXPECT_EQ(uproto_message_parser_state_payload_length, runtime->state);
    ASSERT_FALSE(runtime->parsing_message == NULL);
    EXPECT_EQ(uproto_message_status_parsing, runtime->parsing_message->parse_status);
    EXPECT_FALSE(uproto_parser_has_message_ready(runtime));
    EXPECT_EQ(data.real_value, runtime->parsing_message->resource_id);

    uproto_parser_destroy(&runtime);
}

TEST_P(UprotoParserDynamicValueTests, ParseSingle_PayloadLength) {
    dynamic_test_data data = GetParam();
    if (data.real_value == 0) {
        // Handled in separate test case.
        return;
    }
    uproto_parser_runtime_t* runtime = uproto_parser_create();
    uproto_parser_parse_single(runtime, uproto_message_start);
    uproto_parser_parse_single(runtime, 0b00000001);
    uproto_parser_parse_single(runtime, 0x80);
    uproto_parser_parse_single(runtime, 0x42);

    std::vector<uint8_t> dynamic_buffer = dynamic_split_to_bytes(data.dynamic_value);

    for (const auto& input : dynamic_buffer) {
        EXPECT_EQ(uproto_message_parser_state_payload_length, runtime->state);

        EXPECT_EQ(uproto_message_parser_result_ok, uproto_parser_parse_single(runtime, input));
    }

    EXPECT_EQ(uproto_message_parser_state_payload, runtime->state);
    ASSERT_FALSE(runtime->parsing_message == NULL);
    EXPECT_EQ(uproto_message_status_parsing, runtime->parsing_message->parse_status);
    EXPECT_FALSE(uproto_parser_has_message_ready(runtime));
    EXPECT_EQ(data.real_value, runtime->parsing_message->payload_length);

    uproto_parser_destroy(&runtime);
}

TEST_F(UprotoParserTests, ParseSingle_PayloadLength_ZeroLengthSkipsPayload) {
    uproto_parser_runtime_t* runtime = uproto_parser_create();
    uproto_parser_parse_single(runtime, uproto_message_start);
    uproto_parser_parse_single(runtime, 0b00000001);
    uproto_parser_parse_single(runtime, 0x80);
    uproto_parser_parse_single(runtime, 0x42);

    uproto_parser_parse_single(runtime, 0x00);

    EXPECT_EQ(uproto_message_parser_state_checksum, runtime->state);
    ASSERT_FALSE(runtime->parsing_message == NULL);
    EXPECT_EQ(uproto_message_status_parsing, runtime->parsing_message->parse_status);
    EXPECT_FALSE(uproto_parser_has_message_ready(runtime));
    EXPECT_EQ(0, runtime->parsing_message->payload_length);
    EXPECT_EQ(NULL, runtime->parsing_message->payload);

    uproto_parser_destroy(&runtime);
}

TEST_P(UprotoParserPayloadTests, ParseSingle_Payload) {
    payload_test_data data = GetParam();
    uproto_parser_runtime_t* runtime = uproto_parser_create();
    uproto_parser_parse_single(runtime, uproto_message_start);
    uproto_parser_parse_single(runtime, 0b00000001);
    uproto_parser_parse_single(runtime, 0x80);
    uproto_parser_parse_single(runtime, 0x42);
    for (const auto& value : dynamic_serialize_to_vector(data.payload.size())) {
        uproto_parser_parse_single(runtime, value);
    }

    for (const auto& input : data.payload) {
        EXPECT_EQ(uproto_message_parser_state_payload, runtime->state);

        EXPECT_EQ(uproto_message_parser_result_ok, uproto_parser_parse_single(runtime, input));
    }

    EXPECT_EQ(uproto_message_parser_state_checksum, runtime->state);
    ASSERT_FALSE(runtime->parsing_message == NULL);
    EXPECT_EQ(uproto_message_status_parsing, runtime->parsing_message->parse_status);
    EXPECT_FALSE(uproto_parser_has_message_ready(runtime));
    ASSERT_FALSE(runtime->parsing_message->payload == NULL);
    for (size_t i = 0; i < data.payload.size(); ++i) {
        EXPECT_EQ(data.payload[i], runtime->parsing_message->payload[i]);
    }

    uproto_parser_destroy(&runtime);
}

TEST_F(UprotoParserTests, ParseSingle_Checksum_EmptyMessage) {
    uproto_parser_runtime_t* runtime = uproto_parser_create();
    uproto_parser_parse_single(runtime, uproto_message_start);
    uproto_parser_parse_single(runtime, 0b00000000);
    uproto_parser_parse_single(runtime, 0x00);
    uproto_parser_parse_single(runtime, 0x00);

    EXPECT_EQ(uproto_message_parser_state_checksum, runtime->state);

    EXPECT_EQ(uproto_message_parser_result_ok, uproto_parser_parse_single(runtime, 0xF2));

    ASSERT_FALSE(runtime->parsing_message == NULL);
    EXPECT_EQ(uproto_message_status_parsing, runtime->parsing_message->parse_status);
    EXPECT_FALSE(uproto_parser_has_message_ready(runtime));
    ASSERT_TRUE(runtime->parsing_message->payload == NULL);
    EXPECT_EQ(uproto_message_parser_state_message_end, runtime->state);
    EXPECT_EQ(0xF2, runtime->parsing_message->checksum);

    uproto_parser_destroy(&runtime);
}

TEST_F(UprotoParserTests, ParseSingle_Checksum_ShortPayload) {
    uproto_parser_runtime_t* runtime = uproto_parser_create();
    uproto_parser_parse_single(runtime, uproto_message_start);
    uproto_parser_parse_single(runtime, 0b00000000);
    uproto_parser_parse_single(runtime, 0x42);
    uproto_parser_parse_single(runtime, 0x04);
    uproto_parser_parse_single(runtime, 0x01);
    uproto_parser_parse_single(runtime, 0x02);
    uproto_parser_parse_single(runtime, 0x03);
    uproto_parser_parse_single(runtime, 0x04);

    EXPECT_EQ(uproto_message_parser_state_checksum, runtime->state);

    EXPECT_EQ(uproto_message_parser_result_ok, uproto_parser_parse_single(runtime, 0x04));

    ASSERT_FALSE(runtime->parsing_message == NULL);
    EXPECT_EQ(uproto_message_status_parsing, runtime->parsing_message->parse_status);
    EXPECT_FALSE(uproto_parser_has_message_ready(runtime));
    ASSERT_FALSE(runtime->parsing_message->payload == NULL);
    EXPECT_EQ(uproto_message_parser_state_message_end, runtime->state);
    EXPECT_EQ(0x04, runtime->parsing_message->checksum);

    uproto_parser_destroy(&runtime);
}

TEST_F(UprotoParserTests, ParseSingle_MessageEnd_EmptyMessage) {
    uproto_parser_runtime_t* runtime = uproto_parser_create();
    uproto_parser_parse_single(runtime, uproto_message_start);
    uproto_parser_parse_single(runtime, 0b00000000);
    uproto_parser_parse_single(runtime, 0x00);
    uproto_parser_parse_single(runtime, 0x00);
    uproto_parser_parse_single(runtime, 0xF2);

    EXPECT_EQ(uproto_message_parser_state_message_end, runtime->state);

    EXPECT_EQ(uproto_message_parser_result_message_ready, uproto_parser_parse_single(runtime, uproto_message_end));

    EXPECT_TRUE(runtime->parsing_message == NULL);
    ASSERT_FALSE(runtime->ready_message == NULL);
    EXPECT_EQ(uproto_message_status_ok, runtime->ready_message->parse_status);
    EXPECT_TRUE(uproto_parser_has_message_ready(runtime));
    ASSERT_TRUE(runtime->ready_message->payload == NULL);
    EXPECT_EQ(uproto_message_parser_state_message_start, runtime->state);

    uproto_parser_destroy(&runtime);
}

TEST_F(UprotoParserTests, ParseSingle_MessageEnd_InvalidEnd) {
    uproto_parser_runtime_t* runtime = uproto_parser_create();
    uproto_parser_parse_single(runtime, uproto_message_start);
    uproto_parser_parse_single(runtime, 0b00000000);
    uproto_parser_parse_single(runtime, 0x00);
    uproto_parser_parse_single(runtime, 0x00);
    uproto_parser_parse_single(runtime, 0xF2);

    EXPECT_EQ(uproto_message_parser_state_message_end, runtime->state);

    EXPECT_EQ(uproto_message_parser_result_message_ready, uproto_parser_parse_single(runtime, 0xC3));

    EXPECT_TRUE(runtime->parsing_message == NULL);
    ASSERT_FALSE(runtime->ready_message == NULL);
    EXPECT_EQ(uproto_message_status_end_invalid, runtime->ready_message->parse_status);
    EXPECT_TRUE(uproto_parser_has_message_ready(runtime));
    ASSERT_TRUE(runtime->ready_message->payload == NULL);
    EXPECT_EQ(uproto_message_parser_state_message_start, runtime->state);

    uproto_parser_destroy(&runtime);
}
