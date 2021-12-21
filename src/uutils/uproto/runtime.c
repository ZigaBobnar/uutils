#include "uutils/uproto/runtime.h"
#include <stdio.h>
#include <string.h>
#include "uutils/uproto/parser.h"
#include "uutils/uproto/message.h"
#include "uutils/uproto/version.h"
#include "uutils/dynamic_value.h"

uproto_runtime_t* uproto_runtime_create() {
    uproto_runtime_t* runtime = malloc(sizeof(uproto_runtime_t));

    runtime->device_identifier = NULL;
    runtime->application_identifier = NULL;
    runtime->application_version.major = 1;
    runtime->application_version.minor = 0;
    runtime->application_version.patch = 0;
#if UPROTO_ENABLE_NETWORKING
    runtime->networking_enabled = false;
    runtime->device_id = 0x00;
#endif

    runtime->parser = uproto_parser_create();
    for (uint8_t i = 0; i < UPROTO_RUNTIME_MAX_RECEIVE_ADAPTERS; ++i) {
        runtime->receive_adapters[i] = NULL;
    }
    for (uint8_t i = 0; i < UPROTO_RUNTIME_MAX_RESPOND_ADAPTERS; ++i) {
        runtime->respond_adapters[i] = NULL;
    }

    uproto_runtime_attach_receive_adapter(runtime, &uproto_runtime_system_receive_adapter_action);

    return runtime;
}

void uproto_runtime_destroy(uproto_runtime_t** runtime_ptr) {
    if (runtime_ptr == NULL || *runtime_ptr == NULL) {
        return;
    }

    if ((*runtime_ptr)->parser != NULL) {
        uproto_parser_destroy(&(*runtime_ptr)->parser);
    }

    free(*runtime_ptr);
    *runtime_ptr = NULL;
}

void uproto_runtime_feed_data(uproto_runtime_t* runtime, const uint8_t value) {
    uproto_message_parser_result result = uproto_parser_parse_single(runtime->parser, value);

    switch (result) {
        case uproto_message_parser_result_ok:
            break;
        case uproto_message_parser_result_message_ready: {
            uproto_message_t* message = uproto_parser_get_ready_message(runtime->parser);

            for (uint8_t i = 0; i < UPROTO_RUNTIME_MAX_RECEIVE_ADAPTERS; ++i) {
                if (runtime->receive_adapters[i] != NULL) {
                    if (runtime->receive_adapters[i](runtime, message)) {
                        break;
                    }
                }
            }

            uproto_message_destroy(&message);

            break;
        }
        case uproto_message_parser_result_queue_full: {
            uproto_message_t* message = uproto_parser_get_ready_message(runtime->parser);

            for (uint8_t i = 0; i < UPROTO_RUNTIME_MAX_RECEIVE_ADAPTERS; ++i) {
                if (runtime->receive_adapters[i] != NULL) {
                    if (runtime->receive_adapters[i](runtime, message)) {
                        break;
                    }
                }
            }

            uproto_message_destroy(&message);

            uproto_runtime_feed_data(runtime, value);

            break;
        }
        case uproto_message_parser_result_unknown_error:
            break;
    }
}

void uproto_runtime_feed_message(uproto_runtime_t* runtime, uproto_message_t* message) {
    for (uint8_t i = 0; i < UPROTO_RUNTIME_MAX_RECEIVE_ADAPTERS; ++i) {
        if (runtime->receive_adapters[i] != NULL) {
            if (runtime->receive_adapters[i](runtime, message)) {
                break;
            }
        }
    }

    uproto_message_destroy(&message);
}

void uproto_runtime_respond_with_message(uproto_runtime_t* runtime, uproto_message_t* message) {
    for (uint8_t i = 0; i < UPROTO_RUNTIME_MAX_RESPOND_ADAPTERS; ++i) {
        if (runtime->respond_adapters[i] != NULL) {
            if (runtime->respond_adapters[i](runtime, message)) {
                break;
            }
        }
    }

    uproto_message_destroy(&message);
}

void uproto_runtime_attach_receive_adapter(uproto_runtime_t* runtime, uproto_message_adapter_t adapter) {
    for (uint8_t i = 0; i < UPROTO_RUNTIME_MAX_RECEIVE_ADAPTERS; ++i) {
        if (runtime->receive_adapters[i] == adapter) {
            return;
        }
    }

    for (uint8_t i = 0; i < UPROTO_RUNTIME_MAX_RECEIVE_ADAPTERS; ++i) {
        if (runtime->receive_adapters[i] == NULL) {
            runtime->receive_adapters[i] = adapter;
            return;
        }
    }
}

void uproto_runtime_remove_receive_adapter(uproto_runtime_t* runtime, uproto_message_adapter_t adapter) {
    for (uint8_t i = 0; i < UPROTO_RUNTIME_MAX_RECEIVE_ADAPTERS; ++i) {
        if (runtime->receive_adapters[i] == adapter) {
            runtime->receive_adapters[i] = NULL;
            return;
        }
    }
}

void uproto_runtime_attach_respond_adapter(uproto_runtime_t* runtime, uproto_message_adapter_t adapter) {
    for (uint8_t i = 0; i < UPROTO_RUNTIME_MAX_RESPOND_ADAPTERS; ++i) {
        if (runtime->respond_adapters[i] == adapter) {
            return;
        }
    }

    for (uint8_t i = 0; i < UPROTO_RUNTIME_MAX_RESPOND_ADAPTERS; ++i) {
        if (runtime->respond_adapters[i] == NULL) {
            runtime->respond_adapters[i] = adapter;
            return;
        }
    }
}

void uproto_runtime_remove_respond_adapter(uproto_runtime_t* runtime, uproto_message_adapter_t adapter) {
    for (uint8_t i = 0; i < UPROTO_RUNTIME_MAX_RESPOND_ADAPTERS; ++i) {
        if (runtime->respond_adapters[i] == adapter) {
            runtime->respond_adapters[i] = NULL;
            return;
        }
    }
}

bool uproto_runtime_system_receive_adapter_action(uproto_runtime_t* runtime, uproto_message_t* message) {
    if (uproto_message_is_request(message)) {
        if (message->resource_id == uproto_resource_id_do_nothing) {
            return true;
        } else if (message->resource_id == uproto_resource_id_echo_message) {
            uproto_message_t* response = uproto_message_create_response_for_request(message);
            if (response->payload_length > 0 && response->payload != NULL) {
                size_t length = response->payload_length;
                response->payload = malloc(length);
                for (size_t i = 0; i < length; ++i) {
                    response->payload[i] = message->payload[i];
                }
            }

            uproto_runtime_respond_with_message(runtime, response);
            return true;
        } else if (message->resource_id == uproto_resource_id_system_initialize) {
            return true;
        } else if (message->resource_id == uproto_resource_id_system_status) {
            uproto_message_t* response = uproto_message_create_response_for_request(message);
            response->payload = malloc(1);
            response->payload[0] = 1;

            uproto_runtime_respond_with_message(runtime, response);
            return true;
        } else if (message->resource_id == uproto_protocol_version_major) {
            uproto_message_t* response = uproto_message_create_response_for_request(message);

            uproto_version_info_t version = {
                uproto_protocol_version_major,
                uproto_protocol_version_minor,
                uproto_protocol_version_patch,
            };
            response->payload = uproto_version_info_serialize(&version, &response->payload_length);

            uproto_runtime_respond_with_message(runtime, response);
            return true;
        } else if (message->resource_id == uproto_resource_id_application_version) {
            uproto_message_t* response = uproto_message_create_response_for_request(message);

            response->payload = uproto_version_info_serialize(&runtime->application_version, &response->payload_length);

            uproto_runtime_respond_with_message(runtime, response);
            return true;
        } else if (message->resource_id == uproto_resource_id_device_identifier_string) {
            uproto_message_t* response = uproto_message_create_response_for_request(message);
            static const char* default_device_identifier = "Generic uproto embedded device";

            char* buffer = runtime->device_identifier != NULL ? runtime->device_identifier : default_device_identifier;

            response->payload_length = (dynamic_real)strlen(buffer);
            response->payload = malloc(response->payload_length);
            strcpy(response->payload, buffer);

            uproto_runtime_respond_with_message(runtime, response);
            return true;
        } else if (message->resource_id == uproto_resource_id_application_identifier_string) {
            uproto_message_t* response = uproto_message_create_response_for_request(message);
            static const char* default_application_identifier = "Generic uproto C runtime";

            char* buffer = runtime->application_identifier != NULL ? runtime->application_identifier : default_application_identifier;

            response->payload_length = (dynamic_real)strlen(buffer);
            response->payload = malloc(response->payload_length);
            strcpy(response->payload, buffer);

            uproto_runtime_respond_with_message(runtime, response);
            return true;
        }
    }

    return false;
}

bool uproto_runtime_global_console_receive_adapter_action(uproto_runtime_t* runtime, uproto_message_t* message) {
    printf("=> Runtime receiving message\n");
#pragma warning(suppress: 4477)
    printf("  Message start: 0x%X\n", uproto_message_start);
#pragma warning(suppress: 4477)
    printf("  Properties: 0x%X (Dynamic: 0x%X)\n", message->message_properties, real_to_dynamic(message->message_properties, NULL));
#if UPROTO_ENABLE_NETWORKING
    if (uproto_message_is_using_networking(message)) {
#pragma warning(suppress: 4477)
        printf("  Device id: 0x%X (Dynamic: 0x%X)\n", message->device_id, real_to_dynamic(message->device_id, NULL));
#pragma warning(suppress: 4477)
        printf("  Sender id: 0x%X (Dynamic: 0x%X)\n", message->sender_id, real_to_dynamic(message->sender_id, NULL));
    }
#endif
#pragma warning(suppress: 4477)
    printf("  Resource id: 0x%X (Dynamic: 0x%X)\n", message->resource_id, real_to_dynamic(message->resource_id, NULL));
#pragma warning(suppress: 4477)
    printf("  Payload length: 0x%X (Dynamic: 0x%X)\n", message->payload_length,  real_to_dynamic(message->payload_length, NULL));
    if (message->payload_length <= 0) {
        printf("  Payload: (none)\n");
    } else {
        printf("  Payload: ");
        for (size_t i = 0; i < (size_t)message->payload_length; ++i) {
            printf("0x%X ", message->payload[i]);
        }
        printf("\n");
    }
    printf("  Checksum: 0x%X\n", message->checksum);
    printf("  Calculated checksum: 0x%X\n", uproto_message_calculate_checksum(message));
    printf("  Message end: 0x%X\n", uproto_message_end);

    size_t message_raw_size;
    uint8_t* serialized_message = uproto_message_serialize(message, &message_raw_size);
    printf("  RAW message (%d bytes): ", (int)message_raw_size);
    for (size_t i = 0; i < message_raw_size; ++i) {
        printf("0x%X ", serialized_message[i]);
    }
    printf("\n\n");
    free(serialized_message);

    return false;
}

bool uproto_runtime_global_console_respond_adapter_action(uproto_runtime_t* runtime, uproto_message_t* message) {
    printf("=> Runtime sending message\n");
#pragma warning(suppress: 4477)
    printf("  Message start: 0x%X\n", uproto_message_start);
#pragma warning(suppress: 4477)
    printf("  Properties: 0x%X (Dynamic: 0x%X)\n", message->message_properties, real_to_dynamic(message->message_properties, NULL));
#if UPROTO_ENABLE_NETWORKING
    if (uproto_message_is_using_networking(message)) {
#pragma warning(suppress: 4477)
        printf("  Device id: 0x%X (Dynamic: 0x%X)\n", message->device_id, real_to_dynamic(message->device_id, NULL));
#pragma warning(suppress: 4477)
        printf("  Sender id: 0x%X (Dynamic: 0x%X)\n", message->sender_id, real_to_dynamic(message->sender_id, NULL));
    }
#endif
#pragma warning(suppress: 4477)
    printf("  Resource id: 0x%X (Dynamic: 0x%X)\n", message->resource_id, real_to_dynamic(message->resource_id, NULL));
#pragma warning(suppress: 4477)
    printf("  Payload length: 0x%X (Dynamic: 0x%X)\n", message->payload_length,  real_to_dynamic(message->payload_length, NULL));
    if (message->payload_length <= 0) {
        printf("  Payload: (none)\n");
    } else {
        printf("  Payload: ");
        for (size_t i = 0; i < (size_t)message->payload_length; ++i) {
            printf("0x%X ", message->payload[i]);
        }
        printf("\n");
    }
    printf("  Checksum: 0x%X\n", message->checksum);
    printf("  Calculated checksum: 0x%X\n", uproto_message_calculate_checksum(message));
    printf("  Message end: 0x%X\n", uproto_message_end);

    size_t message_raw_size;
    uint8_t* serialized_message = uproto_message_serialize(message, &message_raw_size);
    printf("  RAW message (%d bytes): ", (int)message_raw_size);
    for (size_t i = 0; i < message_raw_size; ++i) {
        printf("0x%X ", serialized_message[i]);
    }
    free(serialized_message);
    printf("\n\n");

    return false;
}
