#include <gtest/gtest.h>
#include "../test_core.hpp"
#include "uutils/uproto/uproto.h"

class UprotoRuntimeTests : public CoreTest {};

TEST_F(UprotoRuntimeTests, MessagesIntegration) {
    uproto_runtime_t* runtime = uproto_runtime_create();

    uproto_runtime_attach_respond_adapter(runtime, &uproto_runtime_global_console_respond_adapter_action);
    uproto_runtime_attach_receive_adapter(runtime, &uproto_runtime_global_console_receive_adapter_action);
    runtime->receive_adapters[2] = runtime->receive_adapters[0];
    runtime->receive_adapters[0] = runtime->receive_adapters[1];
    runtime->receive_adapters[1] = runtime->receive_adapters[2];
    runtime->receive_adapters[2] = NULL;

    uproto_message_t* message1 = uproto_message_create();
    message1->resource_id = 0x420;

    uproto_runtime_respond_with_message(runtime, message1);

    uproto_message_t* message2 = uproto_message_create();
    message2->resource_id = 0x01;

    uproto_runtime_feed_message(runtime, message2);

    uproto_runtime_destroy(&runtime);
}

TEST_F(UprotoRuntimeTests, RawCommunicationIntegration) {
    uproto_runtime_t* runtime1 = uproto_runtime_create();
    uproto_runtime_attach_respond_adapter(runtime1, &uproto_runtime_global_console_respond_adapter_action);
    /*uproto_runtime_attach_respond_adapter(runtime1, [](uproto_runtime_t* runtime, uproto_message_t* message) {
        return false;
    });*/
    uproto_runtime_attach_receive_adapter(runtime1, &uproto_runtime_global_console_receive_adapter_action);
    /*uproto_runtime_attach_receive_adapter(runtime1, [](uproto_runtime_t* runtime, uproto_message_t* message) {
        return false;
    });*/

    /*uproto_runtime_t* runtime2 = uproto_runtime_create();
    //uproto_runtime_attach_respond_adapter(runtime2, &uproto_runtime_global_console_respond_adapter_action);
    uproto_runtime_attach_respond_adapter(runtime2, [](uproto_runtime_t* runtime, uproto_message_t* message) {
        return false;
    });
    //uproto_runtime_attach_receive_adapter(runtime2, &uproto_runtime_global_console_receive_adapter_action);
    uproto_runtime_attach_receive_adapter(runtime2, [](uproto_runtime_t* runtime, uproto_message_t* message) {
        return false;
    });*/

    runtime1->receive_adapters[2] = runtime1->receive_adapters[0];
    runtime1->receive_adapters[0] = runtime1->receive_adapters[1];
    runtime1->receive_adapters[1] = runtime1->receive_adapters[2];
    runtime1->receive_adapters[2] = NULL;

    /*runtime2->receive_adapters[2] = runtime2->receive_adapters[0];
    runtime2->receive_adapters[0] = runtime2->receive_adapters[1];
    runtime2->receive_adapters[1] = runtime2->receive_adapters[2];
    runtime2->receive_adapters[2] = NULL;*/

    uproto_message_t* message1 = uproto_message_create();
    message1->resource_id = 0x420;

    size_t message1_length;
    uint8_t* message1_buffer = uproto_message_serialize(message1, &message1_length);

    for (size_t i = 0; i < message1_length; ++i) {
        uproto_runtime_feed_data(runtime1, message1_buffer[i]);
    }
    free(message1_buffer);

    //uproto_runtime_respond_with_message(runtime1, message1);

    //uproto_message_t* message2 = uproto_message_create();
    //message2->resource_id = 0x01;

    //uproto_runtime_feed_message(runtime1, message2);

    uproto_runtime_destroy(&runtime1);
    uproto_message_destroy(&message1);
    //uproto_runtime_destroy(&runtime2);
}
