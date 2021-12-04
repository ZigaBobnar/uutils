#include <gtest/gtest.h>
#include "uutils/uproto/uproto.h"

class UprotoRuntimeTests : public testing::Test {
};

TEST_F(UprotoRuntimeTests, Integration) {
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
