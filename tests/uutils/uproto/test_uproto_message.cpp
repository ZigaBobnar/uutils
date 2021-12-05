#include <gtest/gtest.h>
#include "uutils/uproto/uproto.h"
#include "uutils/checksum.h"

class UprotoMessageTests : public testing::Test {
    virtual void TearDown() {
        memory_debug_print_report();
    }
};

TEST_F(UprotoMessageTests, Create) {
    uproto_message_t* message = uproto_message_create();

    ASSERT_EQ(0, message->message_properties);
    ASSERT_EQ(0, message->resource_id);
    ASSERT_EQ(0, message->payload_length);
    ASSERT_EQ(NULL, message->payload);
    ASSERT_EQ(0, message->checksum);
    ASSERT_EQ(uproto_message_status_unknown, message->parse_status);

    free(message);
}

TEST_F(UprotoMessageTests, Destroy) {
    uproto_message_t* message = uproto_message_create();

    uproto_message_destroy(&message);

    ASSERT_EQ(NULL, message);
}

TEST_F(UprotoMessageTests, IsValid) {
    uproto_message_t* message = uproto_message_create();

    ASSERT_TRUE(uproto_message_is_valid(message));

    uproto_message_destroy(&message);

    ASSERT_FALSE(uproto_message_is_valid(message));

    message = new uproto_message_t;

    ASSERT_TRUE(uproto_message_is_valid(message));

    delete message;
}

TEST_F(UprotoMessageTests, IsRequest) {
    uproto_message_t* message = uproto_message_create();

    message->message_properties = 0b00000000;
    EXPECT_TRUE(uproto_message_is_request(message));
    message->message_properties = 0b00000001;
    EXPECT_FALSE(uproto_message_is_request(message));

    uproto_message_destroy(&message);
}

TEST_F(UprotoMessageTests, IsResponse) {
    uproto_message_t* message = uproto_message_create();

    message->message_properties = 0b00000000;
    EXPECT_FALSE(uproto_message_is_response(message));
    message->message_properties = 0b00000001;
    EXPECT_TRUE(uproto_message_is_response(message));

    uproto_message_destroy(&message);
}

TEST_F(UprotoMessageTests, RequestResponseIntegration) {
    uproto_message_t* message = uproto_message_create();

    uproto_message_set_as_request_type(message);

    EXPECT_TRUE(uproto_message_is_request(message));
    EXPECT_FALSE(uproto_message_is_response(message));

    uproto_message_set_as_response_type(message);

    EXPECT_FALSE(uproto_message_is_request(message));
    EXPECT_TRUE(uproto_message_is_response(message));

    uproto_message_set_as_request_type(message);

    EXPECT_TRUE(uproto_message_is_request(message));
    EXPECT_FALSE(uproto_message_is_response(message));

    uproto_message_destroy(&message);
}

TEST_F(UprotoMessageTests, Checksum) {
    uproto_message_t* message = uproto_message_create();
    uproto_message_set_as_request_type(message);
    message->resource_id = 0x08;
    message->payload_length = 0x00;

    uint8_t expected_checksum = uproto_message_start + 0x08;

    EXPECT_EQ(expected_checksum, uproto_message_calculate_checksum(message));
    EXPECT_TRUE(uproto_message_is_checksum_valid(message, expected_checksum));
    EXPECT_FALSE(uproto_message_is_checksum_valid(message, expected_checksum + 1));

    message->resource_id = 0x156553;
    expected_checksum = uproto_message_start + checksum_simple_dynamic_value(0x156553);
    EXPECT_EQ(expected_checksum, uproto_message_calculate_checksum(message));
    EXPECT_TRUE(uproto_message_is_checksum_valid(message, expected_checksum));

    message->payload_length = 3;
    message->payload = new uint8_t[message->payload_length] { 1, 2, 3 };
    expected_checksum = uproto_message_start +
        checksum_simple_dynamic_value(0x156553) +
        3 +
        1 + 2 + 3;
    EXPECT_EQ(expected_checksum, uproto_message_calculate_checksum(message));
    EXPECT_TRUE(uproto_message_is_checksum_valid(message, expected_checksum));

    message->payload_length = 3;
    message->payload = new uint8_t[message->payload_length] { 0 };
    expected_checksum = uproto_message_start +
        checksum_simple_dynamic_value(0x156553) +
        3 + 0;
    EXPECT_EQ(expected_checksum, uproto_message_calculate_checksum(message));
    EXPECT_TRUE(uproto_message_is_checksum_valid(message, expected_checksum));

    message->payload_length = 128;
    expected_checksum = uproto_message_start +
        checksum_simple_dynamic_value(0x156553) +
        128;
    message->payload = new uint8_t[message->payload_length] { 0 };
    for (uint8_t i = 0; i < 128; ++i) {
        message->payload[i] = i;
        expected_checksum += i;
    }
    EXPECT_EQ(expected_checksum, uproto_message_calculate_checksum(message));
    EXPECT_TRUE(uproto_message_is_checksum_valid(message, expected_checksum));

    uproto_message_destroy(&message);
}
