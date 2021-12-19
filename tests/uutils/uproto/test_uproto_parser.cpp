#include <gtest/gtest.h>
#include "../test_core.hpp"
#include "uutils/uproto/uproto.h"
#include "uutils/dynamic_value.hpp"

class UprotoParserTests : public CoreTest {};

class UprotoParserDynamicValueTests : public CoreDataTest<dynamic_test_data> {};
INSTANTIATE_TEST_SUITE_P(DynamicParserTests, UprotoParserDynamicValueTests, dynamic_test_positive_values);

class UprotoParserPayloadTests : public CoreDataTest<payload_test_data> {};
INSTANTIATE_TEST_SUITE_P(PayloadParserTests, UprotoParserPayloadTests, payload_test_values);

TEST_F(UprotoParserTests, Initialization) {
    uproto_parser_t* parser = uproto_parser_create();

    EXPECT_EQ(uproto_message_parser_state_message_start, parser->state);
    EXPECT_EQ(NULL, parser->parsing_message);
    EXPECT_EQ(NULL, parser->ready_message);

    uproto_parser_destroy(&parser);

    EXPECT_EQ(NULL, parser);
}

TEST_F(UprotoParserTests, ParseSingle_ZeroData_NoParsing) {
    uproto_parser_t* parser = uproto_parser_create();

    for (size_t i = 0; i < 50; i++) {
        EXPECT_EQ(uproto_message_parser_result_ok, uproto_parser_parse_single(parser, 0x00));
        EXPECT_TRUE(parser->parsing_message == NULL);
        EXPECT_FALSE(uproto_parser_has_message_ready(parser));
        ASSERT_EQ(uproto_message_parser_state_message_start, parser->state);
    }

    uproto_parser_destroy(&parser);
}

TEST_F(UprotoParserTests, ParseSingle_MessageStart) {
    uproto_parser_t* parser = uproto_parser_create();

    EXPECT_EQ(uproto_message_parser_result_ok, uproto_parser_parse_single(parser, uproto_message_start));
    EXPECT_EQ(uproto_message_parser_state_message_properties, parser->state);
    ASSERT_FALSE(parser->parsing_message == NULL);
    EXPECT_EQ(uproto_message_status_parsing, parser->parsing_message->parse_status);
    EXPECT_FALSE(uproto_parser_has_message_ready(parser));

    uproto_parser_destroy(&parser);
}

TEST_F(UprotoParserTests, ParseSingle_MessageStart_InvalidStart) {
    uproto_parser_t* parser = uproto_parser_create();

    EXPECT_EQ(uproto_message_parser_result_ok, uproto_parser_parse_single(parser, 0x43));
    EXPECT_EQ(uproto_message_parser_state_message_start, parser->state);
    EXPECT_TRUE(parser->parsing_message == NULL);
    EXPECT_FALSE(uproto_parser_has_message_ready(parser));

    uproto_parser_destroy(&parser);
}

TEST_F(UprotoParserTests, ParseSingle_MessageStart_DelayedStart) {
    uproto_parser_t* parser = uproto_parser_create();

    for (size_t i = 0; i < 50; i++) {
        EXPECT_EQ(uproto_message_parser_result_ok, uproto_parser_parse_single(parser, 0x00));
        EXPECT_TRUE(parser->parsing_message == NULL);
        EXPECT_FALSE(uproto_parser_has_message_ready(parser));
        ASSERT_EQ(uproto_message_parser_state_message_start, parser->state);
    }

    EXPECT_EQ(uproto_message_parser_result_ok, uproto_parser_parse_single(parser, uproto_message_start));
    EXPECT_EQ(uproto_message_parser_state_message_properties, parser->state);
    ASSERT_FALSE(parser->parsing_message == NULL);
    EXPECT_EQ(uproto_message_status_parsing, parser->parsing_message->parse_status);
    EXPECT_FALSE(uproto_parser_has_message_ready(parser));

    uproto_parser_destroy(&parser);
}

TEST_F(UprotoParserTests, ParseSingle_MessageProperties_RequestMessage) {
    uproto_parser_t* parser = uproto_parser_create();
    uproto_parser_parse_single(parser, uproto_message_start);

    EXPECT_EQ(uproto_message_parser_result_ok, uproto_parser_parse_single(parser, 0b00000000));
    EXPECT_EQ(uproto_message_parser_state_resource_id, parser->state);
    ASSERT_FALSE(parser->parsing_message == NULL);
    EXPECT_EQ(uproto_message_status_parsing, parser->parsing_message->parse_status);
    EXPECT_FALSE(uproto_parser_has_message_ready(parser));
    EXPECT_EQ(0x00, parser->parsing_message->message_properties);

    uproto_parser_destroy(&parser);
}

TEST_F(UprotoParserTests, ParseSingle_MessageProperties_ResponseMessage) {
    uproto_parser_t* parser = uproto_parser_create();
    uproto_parser_parse_single(parser, uproto_message_start);

    EXPECT_EQ(uproto_message_parser_result_ok, uproto_parser_parse_single(parser, 0b00000001));
    EXPECT_EQ(uproto_message_parser_state_resource_id, parser->state);
    ASSERT_FALSE(parser->parsing_message == NULL);
    EXPECT_EQ(uproto_message_status_parsing, parser->parsing_message->parse_status);
    EXPECT_FALSE(uproto_parser_has_message_ready(parser));
    EXPECT_EQ(0x01, parser->parsing_message->message_properties);

    uproto_parser_destroy(&parser);
}

TEST_F(UprotoParserTests, ParseSingle_MessageProperties_ChecksumEnabled) {
    uproto_parser_t* parser = uproto_parser_create();
    uproto_parser_parse_single(parser, uproto_message_start);

    EXPECT_EQ(uproto_message_parser_result_ok, uproto_parser_parse_single(parser, 0b00000010));
    EXPECT_TRUE(uproto_message_has_checksum(parser->parsing_message));
    EXPECT_FALSE(uproto_message_skips_checksum(parser->parsing_message));

    uproto_parser_destroy(&parser);
}

TEST_F(UprotoParserTests, ParseSingle_MessageProperties_ChecksumDisabled) {
    uproto_parser_t* parser = uproto_parser_create();
    uproto_parser_parse_single(parser, uproto_message_start);

    EXPECT_EQ(uproto_message_parser_result_ok, uproto_parser_parse_single(parser, 0b00000000));
    EXPECT_FALSE(uproto_message_has_checksum(parser->parsing_message));
    EXPECT_TRUE(uproto_message_skips_checksum(parser->parsing_message));

    uproto_parser_destroy(&parser);
}

TEST_P(UprotoParserDynamicValueTests, ParseSingle_ResourceId) {
    dynamic_test_data data = GetParam();
    uproto_parser_t* parser = uproto_parser_create();
    uproto_parser_parse_single(parser, uproto_message_start);
    uproto_parser_parse_single(parser, 0b00000001);

    std::vector<uint8_t> dynamic_buffer = dynamic_split_to_bytes(data.dynamic_value);

    for (const auto& input : dynamic_buffer) {
        EXPECT_EQ(uproto_message_parser_state_resource_id, parser->state);

        EXPECT_EQ(uproto_message_parser_result_ok, uproto_parser_parse_single(parser, input));
    }

    EXPECT_EQ(uproto_message_parser_state_payload_length, parser->state);
    ASSERT_FALSE(parser->parsing_message == NULL);
    EXPECT_EQ(uproto_message_status_parsing, parser->parsing_message->parse_status);
    EXPECT_FALSE(uproto_parser_has_message_ready(parser));
    EXPECT_EQ(data.real_value, parser->parsing_message->resource_id);

    uproto_parser_destroy(&parser);
}

TEST_P(UprotoParserDynamicValueTests, ParseSingle_PayloadLength) {
    dynamic_test_data data = GetParam();
    if (data.real_value == 0) {
        // Handled in separate test case.
        return;
    }
    uproto_parser_t* parser = uproto_parser_create();
    uproto_parser_parse_single(parser, uproto_message_start);
    uproto_parser_parse_single(parser, 0b00000001);
    uproto_parser_parse_single(parser, 0x20);

    std::vector<uint8_t> dynamic_buffer = dynamic_split_to_bytes(data.dynamic_value);

    for (const auto& input : dynamic_buffer) {
        EXPECT_EQ(uproto_message_parser_state_payload_length, parser->state);

        EXPECT_EQ(uproto_message_parser_result_ok, uproto_parser_parse_single(parser, input));
    }

    EXPECT_EQ(uproto_message_parser_state_payload, parser->state);
    ASSERT_FALSE(parser->parsing_message == NULL);
    EXPECT_EQ(uproto_message_status_parsing, parser->parsing_message->parse_status);
    EXPECT_FALSE(uproto_parser_has_message_ready(parser));
    EXPECT_EQ(data.real_value, parser->parsing_message->payload_length);

    uproto_parser_destroy(&parser);
}

TEST_F(UprotoParserTests, ParseSingle_PayloadLength_ZeroLengthSkipsPayload) {
    uproto_parser_t* parser = uproto_parser_create();
    uproto_parser_parse_single(parser, uproto_message_start);
    uproto_parser_parse_single(parser, 0b00000001);
    uproto_parser_parse_single(parser, 0x20);

    uproto_parser_parse_single(parser, 0x00);

    EXPECT_EQ(uproto_message_parser_state_checksum, parser->state);
    ASSERT_FALSE(parser->parsing_message == NULL);
    EXPECT_EQ(uproto_message_status_parsing, parser->parsing_message->parse_status);
    EXPECT_FALSE(uproto_parser_has_message_ready(parser));
    EXPECT_EQ(0, parser->parsing_message->payload_length);
    EXPECT_EQ(NULL, parser->parsing_message->payload);

    uproto_parser_destroy(&parser);
}

TEST_P(UprotoParserPayloadTests, ParseSingle_Payload) {
    payload_test_data data = GetParam();
    uproto_parser_t* parser = uproto_parser_create();
    uproto_parser_parse_single(parser, uproto_message_start);
    uproto_parser_parse_single(parser, 0b00000001);
    uproto_parser_parse_single(parser, 0x20);
    for (const auto& value : real_to_dynamic_vector((dynamic_real)data.payload.size())) {
        uproto_parser_parse_single(parser, value);
    }

    for (const auto& input : data.payload) {
        EXPECT_EQ(uproto_message_parser_state_payload, parser->state);

        EXPECT_EQ(uproto_message_parser_result_ok, uproto_parser_parse_single(parser, input));
    }

    EXPECT_EQ(uproto_message_parser_state_checksum, parser->state);
    ASSERT_FALSE(parser->parsing_message == NULL);
    EXPECT_EQ(uproto_message_status_parsing, parser->parsing_message->parse_status);
    EXPECT_FALSE(uproto_parser_has_message_ready(parser));
    ASSERT_FALSE(parser->parsing_message->payload == NULL);
    for (size_t i = 0; i < data.payload.size(); ++i) {
        EXPECT_EQ(data.payload[i], parser->parsing_message->payload[i]);
    }

    uproto_parser_destroy(&parser);
}

TEST_F(UprotoParserTests, ParseSingle_Checksum_EmptyMessage) {
    uproto_parser_t* parser = uproto_parser_create();
    uproto_parser_parse_single(parser, uproto_message_start);
    uproto_parser_parse_single(parser, 0b00000010);
    uproto_parser_parse_single(parser, 0x00);
    uproto_parser_parse_single(parser, 0x00);

    EXPECT_EQ(uproto_message_parser_state_checksum, parser->state);

    EXPECT_EQ(uproto_message_parser_result_ok, uproto_parser_parse_single(parser, 0xF2));

    ASSERT_FALSE(parser->parsing_message == NULL);
    EXPECT_EQ(uproto_message_status_parsing, parser->parsing_message->parse_status);
    EXPECT_FALSE(uproto_parser_has_message_ready(parser));
    ASSERT_TRUE(parser->parsing_message->payload == NULL);
    EXPECT_EQ(uproto_message_parser_state_message_end, parser->state);
    EXPECT_EQ(0xF2, parser->parsing_message->checksum);

    uproto_parser_destroy(&parser);
}

TEST_F(UprotoParserTests, ParseSingle_Checksum_ShortPayload) {
    uproto_parser_t* parser = uproto_parser_create();
    uproto_parser_parse_single(parser, uproto_message_start);
    uproto_parser_parse_single(parser, 0b00000010);
    uproto_parser_parse_single(parser, 0x30);
    uproto_parser_parse_single(parser, 0x04);
    uproto_parser_parse_single(parser, 0x01);
    uproto_parser_parse_single(parser, 0x02);
    uproto_parser_parse_single(parser, 0x03);
    uproto_parser_parse_single(parser, 0x04);

    EXPECT_EQ(uproto_message_parser_state_checksum, parser->state);

    EXPECT_EQ(uproto_message_parser_result_ok, uproto_parser_parse_single(parser, 0x04));

    ASSERT_FALSE(parser->parsing_message == NULL);
    EXPECT_EQ(uproto_message_status_parsing, parser->parsing_message->parse_status);
    EXPECT_FALSE(uproto_parser_has_message_ready(parser));
    ASSERT_FALSE(parser->parsing_message->payload == NULL);
    EXPECT_EQ(uproto_message_parser_state_message_end, parser->state);
    EXPECT_EQ(0x04, parser->parsing_message->checksum);

    uproto_parser_destroy(&parser);
}

TEST_F(UprotoParserTests, ParseSingle_MessageEnd_EmptyMessage) {
    uproto_parser_t* parser = uproto_parser_create();
    uproto_parser_parse_single(parser, uproto_message_start);
    uproto_parser_parse_single(parser, 0b00000010);
    uproto_parser_parse_single(parser, 0x00);
    uproto_parser_parse_single(parser, 0x00);
    uproto_parser_parse_single(parser, 0xF2);

    EXPECT_EQ(uproto_message_parser_state_message_end, parser->state);

    EXPECT_EQ(uproto_message_parser_result_message_ready, uproto_parser_parse_single(parser, uproto_message_end));

    EXPECT_TRUE(parser->parsing_message == NULL);
    ASSERT_FALSE(parser->ready_message == NULL);
    EXPECT_EQ(uproto_message_status_ok, parser->ready_message->parse_status);
    EXPECT_TRUE(uproto_parser_has_message_ready(parser));
    ASSERT_TRUE(parser->ready_message->payload == NULL);
    EXPECT_EQ(uproto_message_parser_state_message_start, parser->state);

    uproto_parser_destroy(&parser);
}

TEST_F(UprotoParserTests, ParseSingle_MessageEnd_InvalidEnd) {
    uproto_parser_t* parser = uproto_parser_create();
    uproto_parser_parse_single(parser, uproto_message_start);
    uproto_parser_parse_single(parser, 0b00000010);
    uproto_parser_parse_single(parser, 0x00);
    uproto_parser_parse_single(parser, 0x00);
    uproto_parser_parse_single(parser, 0xF2);

    EXPECT_EQ(uproto_message_parser_state_message_end, parser->state);

    EXPECT_EQ(uproto_message_parser_result_message_ready, uproto_parser_parse_single(parser, 0xC3));

    EXPECT_TRUE(parser->parsing_message == NULL);
    ASSERT_FALSE(parser->ready_message == NULL);
    EXPECT_EQ(uproto_message_status_end_invalid, parser->ready_message->parse_status);
    EXPECT_TRUE(uproto_parser_has_message_ready(parser));
    ASSERT_TRUE(parser->ready_message->payload == NULL);
    EXPECT_EQ(uproto_message_parser_state_message_start, parser->state);

    uproto_parser_destroy(&parser);
}

TEST_F(UprotoParserTests, GetReadyMessage) {
    uproto_parser_t* parser = uproto_parser_create();
    uproto_message_t* message = uproto_message_create();

    parser->ready_message = message;

    EXPECT_TRUE(uproto_parser_has_message_ready(parser));
    uproto_message_t* ready_message = uproto_parser_get_ready_message(parser);
    EXPECT_EQ(message, ready_message);
    EXPECT_TRUE(parser->ready_message == NULL);

    uproto_message_destroy(&ready_message);
    uproto_parser_destroy(&parser);
}
