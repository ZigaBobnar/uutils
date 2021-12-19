#ifndef UUTILS_UPROTO_PARSER_H_
#define UUTILS_UPROTO_PARSER_H_

#include "uutils/uproto/core.h"
#include "uutils/dynamic_value.h"

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

struct uproto_parser_t {
    struct uproto_message_t* ready_message;
    struct uproto_message_t* parsing_message;

    uproto_message_parser_state state;

    dynamic_state_t message_properties_state;
    dynamic_state_t resource_id_state;
    dynamic_state_t payload_length_state;

    size_t current_payload_position;
    size_t current_message_payload_length;
};

uproto_parser_t* uproto_parser_create();

void uproto_parser_destroy(uproto_parser_t** parser_ptr);

uproto_message_parser_result uproto_parser_parse_single(uproto_parser_t* parser, const uint8_t value);

uproto_message_parser_result uproto_parser_parse_multi(uproto_parser_t* parser, const uint8_t* buffer, const size_t length);

bool uproto_parser_has_message_ready(uproto_parser_t* parser);

uproto_message_t* uproto_parser_get_ready_message(uproto_parser_t* parser);

__EXTERN_C_END

#endif  // UUTILS_UPROTO_PARSER_H_
