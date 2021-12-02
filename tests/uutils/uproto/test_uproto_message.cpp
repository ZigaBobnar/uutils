#include <gtest/gtest.h>
#include <uutils/uproto/uproto.h>

class UprotoMessageTests : public testing::Test {
};

TEST_F(UprotoMessageTests, Create) {
    uproto_message_t* message = uproto_message_create();

    ASSERT_EQ(0, message->message_properties);
    ASSERT_EQ(0, message->resource_id);
    ASSERT_EQ(0, message->payload_length);
    ASSERT_EQ(NULL, message->payload);
    ASSERT_EQ(0, message->checksum);
    ASSERT_EQ(uproto_message_unknown, message->parse_status);

    delete message;
}
