#include "uutils/uproto/parser.h"
#include "uutils/uproto/message.h"
#include "uutils/dynamic_value.h"

uproto_parser_t* uproto_parser_create() {
    uproto_parser_t* parser = malloc(sizeof(uproto_parser_t));

    parser->state = uproto_message_parser_state_message_start;
    parser->parsing_message = NULL;
    parser->ready_message = NULL;
    parser->message_properties_state.required_num = 0;
    parser->resource_id_state.required_num = 0;
    parser->payload_length_state.required_num = 0;

    return parser;
}

void uproto_parser_destroy(uproto_parser_t** parser_ptr) {
    if (parser_ptr == NULL || *parser_ptr == NULL) {
        return;
    }

    if ((*parser_ptr)->parsing_message != NULL) {
        uproto_message_destroy(&(*parser_ptr)->parsing_message);
    }

    if ((*parser_ptr)->ready_message != NULL) {
        uproto_message_destroy(&(*parser_ptr)->ready_message);
    }

    free(*parser_ptr);
    *parser_ptr = NULL;
}

uproto_message_parser_result uproto_parser_parse_single(uproto_parser_t* parser, const uint8_t value) {
    switch (parser->state) {
    case uproto_message_parser_state_message_start:
        if (value == uproto_message_start) {
            parser->parsing_message = uproto_message_create();
            parser->parsing_message->parse_status = uproto_message_status_parsing;

            parser->state = uproto_message_parser_state_message_properties;
        }
        break;
    case uproto_message_parser_state_message_properties: {
        if (dynamic_to_real_stateful(&parser->message_properties_state, value, &parser->parsing_message->message_properties)) {
            parser->state = uproto_message_parser_state_resource_id;
        }

        break;
    }
    case uproto_message_parser_state_resource_id: {
        if (dynamic_to_real_stateful(&parser->resource_id_state, value, &parser->parsing_message->resource_id)) {
            parser->state = uproto_message_parser_state_payload_length;
        }

        break;
    }
    case uproto_message_parser_state_payload_length: {
        if (dynamic_to_real_stateful(&parser->payload_length_state, value, &parser->parsing_message->payload_length)) {
            parser->state = parser->parsing_message->payload_length > 0 ? 
                uproto_message_parser_state_payload :
                uproto_message_parser_state_checksum;
        }

        break;
    }
    case uproto_message_parser_state_payload: {
        static size_t payload_position = 0;
        static size_t message_payload_length = 0;

        if (payload_position == 0) {
             if (parser->parsing_message->payload_length == 0) {
                 // Invalid state.
             }

             message_payload_length = parser->parsing_message->payload_length;
             parser->parsing_message->payload = malloc(message_payload_length);
        }

        parser->parsing_message->payload[payload_position++] = value;

        if (payload_position >= parser->parsing_message->payload_length) {
            parser->state = uproto_message_parser_state_checksum;
            payload_position = 0;
        }

        break;
    }
    case uproto_message_parser_state_checksum:
        if (!uproto_message_has_checksum(parser->parsing_message)) {
            parser->state = uproto_message_parser_state_message_end;

            return uproto_parser_parse_single(parser, value);
        }

        parser->parsing_message->checksum = value;
        parser->state = uproto_message_parser_state_message_end;

        break;
    case uproto_message_parser_state_message_end:
        parser->ready_message = parser->parsing_message;
        parser->parsing_message = NULL;

        parser->state = uproto_message_parser_state_message_start;

        if (value == uproto_message_end) {
            parser->ready_message->parse_status = uproto_message_status_ok;
        } else {
            parser->ready_message->parse_status = uproto_message_status_end_invalid;
        }

        return uproto_message_parser_result_message_ready;
        break;
    }

    return uproto_message_parser_result_ok;
}

uproto_message_parser_result uproto_parser_parse_multi(uproto_parser_t* parser, const uint8_t* buffer, const size_t length) {
    return uproto_message_parser_result_unknown_error;
}

bool uproto_parser_has_message_ready(uproto_parser_t* parser) {
    return parser->ready_message != NULL;
}

uproto_message_t* uproto_parser_get_ready_message(uproto_parser_t* parser) {
    if (parser->ready_message == NULL) {
        return NULL;
    }

    uproto_message_t* message = parser->ready_message;
    parser->ready_message = NULL;

    return message;
}
