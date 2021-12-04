#ifndef UUTILS_UPROTO_PARSER_H_
#define UUTILS_UPROTO_PARSER_H_

#include "uutils/uproto/core.h"

__EXTERN_C_BEGIN

typedef enum {
    uproto_message_parser_state_message_start = 0,
    uproto_message_parser_state_message_properties,
    uproto_message_parser_state_resource_id,
    uproto_message_parser_state_payload_length,
    uproto_message_parser_state_payload,
    uproto_message_parser_state_checksum,
    uproto_message_parser_state_message_end,
} uproto_message_parser_state;

typedef enum {
    uproto_message_parser_result_ok,
    uproto_message_parser_result_message_ready,
    uproto_message_parser_result_queue_full,
    uproto_message_parser_result_unknown_error,
} uproto_message_parser_result;

struct uproto_parser_runtime_t {
    struct uproto_message_t* ready_message;
    struct uproto_message_t* parsing_message;

    uproto_message_parser_state state;
};

uproto_parser_runtime_t* uproto_parser_create();

void uproto_parser_destroy(uproto_parser_runtime_t** runtime_ptr);

uproto_message_parser_result uproto_parser_parse_single(uproto_parser_runtime_t* runtime, const uint8_t value);

uproto_message_parser_result uproto_parser_parse_multi(uproto_parser_runtime_t* runtime, const uint8_t* buffer, const size_t length);

bool uproto_parser_has_message_ready(uproto_parser_runtime_t* runtime);

uproto_message_t* uproto_parser_get_ready_message(uproto_parser_runtime_t* runtime);

__EXTERN_C_END

#endif  // UUTILS_UPROTO_PARSER_H_
