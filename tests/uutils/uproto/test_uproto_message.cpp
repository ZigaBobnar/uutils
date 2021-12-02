#include <gtest/gtest.h>
#include "uutils/uproto/uproto.h"

class UprotoMessageTests : public testing::Test {
};

TEST_F(UprotoMessageTests, Create) {
    uproto_message_t* message = uproto_message_create();

    ASSERT_EQ(0, message->message_properties);
    ASSERT_EQ(0, message->resource_id);
    ASSERT_EQ(0, message->payload_length);
    ASSERT_EQ(NULL, message->payload);
    ASSERT_EQ(0, message->checksum);
    ASSERT_EQ(uproto_message_status_unknown, message->parse_status);

    delete message;
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
