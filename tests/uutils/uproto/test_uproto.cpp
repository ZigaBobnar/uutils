#include <gtest/gtest.h>
#include "../test_core.hpp"
#include "uutils/uproto/uproto.h"

class UprotoTests : public CoreTest {};

TEST_F(UprotoTests, RealWorldUsage) {
    uproto_runtime_t* runtime = uproto_runtime_create();
    uproto_runtime_attach_receive_adapter(runtime, [](uproto_runtime_t* runtime, uproto_message_t* message) {
        puts("Received message...");

        if (message->resource_id == 56) {
            puts("=> RGB update message");

            uint8_t red = message->payload[0];
            uint8_t green = message->payload[1];
            uint8_t blue = message->payload[2];

            // analogWrite(RED_PIN, red);
            // analogWrite(GREEN_PIN, green);
            // analogWrite(BLUE_PIN, blue);

            uproto_message_t* response = uproto_message_create();
            response->message_properties = message->message_properties;
            uproto_message_set_as_response_type(response);
            response->resource_id = message->resource_id;

            response->payload = (uint8_t*)malloc(1);
            response->payload[0] = 0x01;
            response->payload_length = 1;

            uproto_runtime_respond_with_message(runtime, response);

            return true;
        }

        return false;
    });
    uproto_runtime_attach_respond_adapter(runtime, [](uproto_runtime_t* runtime, uproto_message_t* message) {
        size_t message_raw_size;
        uint8_t* serialized_message = uproto_message_serialize(message, &message_raw_size);
        printf("Responding with message...\n");
        for (size_t i = 0; i < message_raw_size; ++i) {
            printf("%X", serialized_message[i]);
        }
        free(serialized_message);
        puts("\n");

        return true;
    });

    uint8_t data_buffer[] = {
        0x33,
        0x02,
        0x38,
        0x03,
        0x96,
        0x96,
        0x96,
        0xcc,
    };
    for (size_t n = 0; n < 20; ++n) {
        for (size_t i = 0; i < 8; ++i) {
            uproto_runtime_feed_data(runtime, data_buffer[i]);
        }
    }

    uproto_runtime_destroy(&runtime);
}
