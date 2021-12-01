#ifndef UUTILS_UPROTO_UPROTO_H_
#define UUTILS_UPROTO_UPROTO_H_

#include "uutils/common.h"

/**
 * uproto message structure:
 *
 * [1b] message_start = 0b00110011          -> uproto message start and type designator
 *                                              If the message start is not recognized, it will be discarded.
 *
 * [1b] message_params = 0b0000000r         -> The message parameters.
 *                                              r specifies whether the message is request (r==0) or response (r==1)
 *
 * [1b...4b] resource_id                    -> The resource ID, first 128 (0-127) resource IDs are reserved for protocol use, anything above that can be used by application code.
 *
 * [1b...4b] payload_length                 -> Total payload length in bytes.
 *
 * [payload_length] payload                 -> The payload. Payload can have its own underlying structure that is processed in handler.
 *
 * [1b] checksum                            -> Checksum of complete message - if not valid the message is marked as invalid and to be discarded.
 *                                              Calculated as simple sum of complete message and taken the lower 8 bits. Elements present in checksum:
 *                                                  message_start + message_params + resource_id + payload_length + payload + checksum
 *
 * [1b] message_end = 0b11001100            -> uproto message end.
 *
 * Minimum message length (empty payload): 1b+1b+1b+1b+0b+1b+1b = 6b
 *
 */

const static uint16_t uproto_runtime_version_major = 0x00;
const static uint16_t uproto_runtime_version_minor = 0x00;
const static uint16_t uproto_runtime_version_patch = 0x01;

const static uint8_t uproto_message_start = 0b00110011;
const static uint8_t uproto_message_end = 0b11001100;

const static uint8_t message_property_request_response_mask = 0x1;
const static uint8_t uproto_message_properties_current_empty_mask = 0b11111110;

typedef enum {
    uproto_message_ok = 0,
    uproto_message_parsing,
    uproto_message_unsupported,
    uproto_message_length_invalid,
    uproto_message_start_invalid,
    uproto_message_end_invalid,
    uproto_message_payload_length_mismatch,
    uproto_message_checksum_mismatch,
} uproto_parse_status;

typedef struct {
    uint8_t message_properties;
    uint32_t resource_id;
    uint32_t payload_length;
    uint8_t* payload;
    uint8_t checksum;

    uproto_parse_status parse_status;
} uproto_message_t;

typedef enum {
    uproto_message_parser_state_message_start,
    uproto_message_parser_state_message_params,
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

typedef struct {
    uproto_message_t* ready_message;
    uproto_message_t* parsing_message;

    uproto_message_parser_state state;
} uproto_parser_runtime_t;

uproto_message_t* uproto_message_init();
void uproto_message_destroy(uproto_message_t* message);
bool uproto_message_is_valid(const uproto_message_t* message);

uproto_message_parser_result uproto_parser_parse_single(uproto_parser_runtime_t* runtime, const uint8_t value);
uproto_message_parser_result uproto_parser_parse_multi(uproto_parser_runtime_t* runtime, const uint8_t* buffer, const size_t length);
bool uproto_parser_has_message_ready(uproto_parser_runtime_t* runtime);

#endif  // UUTILS_UPROTO_UPROTO_H_
