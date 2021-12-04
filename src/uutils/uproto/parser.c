#include "uutils/uproto/parser.h"
#include "uutils/uproto/message.h"
#include "uutils/dynamic_value.h"

uproto_parser_runtime_t* uproto_parser_create() {
    uproto_parser_runtime_t* runtime = malloc(sizeof(uproto_parser_runtime_t));

    runtime->state = uproto_message_parser_state_message_start;
    runtime->parsing_message = NULL;
    runtime->ready_message = NULL;

    return runtime;
}

void uproto_parser_destroy(uproto_parser_runtime_t** runtime_ptr) {
    if (runtime_ptr == NULL || *runtime_ptr == NULL) {
        return;
    }

    if ((*runtime_ptr)->parsing_message != NULL) {
        uproto_message_destroy(&(*runtime_ptr)->parsing_message);
    }

    if ((*runtime_ptr)->ready_message != NULL) {
        uproto_message_destroy(&(*runtime_ptr)->ready_message);
    }

    free(*runtime_ptr);
    *runtime_ptr = NULL;
}

uproto_message_parser_result uproto_parser_parse_single(uproto_parser_runtime_t* runtime, const uint8_t value) {
    switch (runtime->state) {
    case uproto_message_parser_state_message_start:
        if (value == uproto_message_start) {
            runtime->parsing_message = uproto_message_create();
            runtime->parsing_message->parse_status = uproto_message_status_parsing;

            runtime->state = uproto_message_parser_state_message_properties;
        }
        break;
    case uproto_message_parser_state_message_properties:
        runtime->parsing_message->message_properties = value;

        runtime->state = uproto_message_parser_state_resource_id;
        break;
    case uproto_message_parser_state_resource_id: {
        static uint8_t required_num = 0;
        static uint8_t cached_num;
        static uint8_t cache[8];

        if (required_num == 0) {
            required_num = dynamic_parse_get_required_bytes(value);
            cached_num = 0;
        }

        if (required_num == 1) {
            runtime->parsing_message->resource_id = (uint32_t)dynamic_parse(value, NULL);

            runtime->state = uproto_message_parser_state_payload_length;
            required_num = 0;
        }

        cache[cached_num++] = value;

        if (cached_num == required_num) {
            runtime->parsing_message->resource_id = (uint32_t)dynamic_parse_buffer(cache, NULL);

            runtime->state = uproto_message_parser_state_payload_length;
            required_num = 0;
        }
        break;
    }
    case uproto_message_parser_state_payload_length: {
        static uint8_t required_num = 0;
        static uint8_t cached_num;
        static uint8_t cache[8];

        if (required_num == 0) {
            required_num = dynamic_parse_get_required_bytes(value);
            cached_num = 0;

            if (required_num == 1) {
                runtime->parsing_message->payload_length = (uint32_t)dynamic_parse(value, NULL);

                runtime->state = runtime->parsing_message->payload_length > 0 ? 
                    uproto_message_parser_state_payload :
                    uproto_message_parser_state_checksum;

                required_num = 0;
            }
        }

        cache[cached_num++] = value;

        if (cached_num == required_num) {
            runtime->parsing_message->payload_length = (uint32_t)dynamic_parse_buffer(cache, NULL);

            runtime->state = uproto_message_parser_state_payload;
            required_num = 0;
        }
        break;
    }
    case uproto_message_parser_state_payload: {
        static size_t payload_position = 0;
        static size_t message_payload_length = 0;

        if (payload_position == 0) {
             if (runtime->parsing_message->payload_length == 0) {
                 // Invalid state.
             }

             message_payload_length = runtime->parsing_message->payload_length;
             runtime->parsing_message->payload = malloc(message_payload_length);
        }

        runtime->parsing_message->payload[payload_position++] = value;

        if (payload_position >= runtime->parsing_message->payload_length) {
            runtime->state = uproto_message_parser_state_checksum;
            payload_position = 0;
        }

        break;
    }
    case uproto_message_parser_state_checksum:
        if (!uproto_message_has_checksum(runtime->parsing_message)) {
            runtime->state = uproto_message_end;

            return uproto_parser_parse_single(runtime, value);
        }

        runtime->parsing_message->checksum = value;
        runtime->state = uproto_message_parser_state_message_end;

        break;
    case uproto_message_parser_state_message_end:
        runtime->ready_message = runtime->parsing_message;
        runtime->parsing_message = NULL;

        runtime->state = uproto_message_parser_state_message_start;

        if (value == uproto_message_end) {
            runtime->ready_message->parse_status = uproto_message_status_ok;
        } else {
            runtime->ready_message->parse_status = uproto_message_status_end_invalid;
        }

        return uproto_message_parser_result_message_ready;
        break;
    }

    return uproto_message_parser_result_ok;
}

uproto_message_parser_result uproto_parser_parse_multi(uproto_parser_runtime_t* runtime, const uint8_t* buffer, const size_t length) {
    return uproto_message_parser_result_unknown_error;
}

bool uproto_parser_has_message_ready(uproto_parser_runtime_t* runtime) {
    return runtime->ready_message != NULL;
}

uproto_message_t* uproto_parser_get_ready_message(uproto_parser_runtime_t* runtime) {
    if (runtime->ready_message == NULL) {
        return NULL;
    }

    // TODO: TESTS!!
    uproto_parser_runtime_t* message = runtime->ready_message;
    runtime->ready_message = NULL;

    return message;
}
