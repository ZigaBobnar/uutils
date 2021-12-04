#ifndef UUTILS_UPROTO_MESSAGE_H_
#define UUTILS_UPROTO_MESSAGE_H_

#include "uutils/uproto/core.h"

/**
 * uproto message structure:
 *
 * [1b] message_start = 0b00110011          -> uproto message start and type designator
 *                                              If the message start is not recognized, it will be discarded.
 *
 * [1b] message_params = 0b000000cr         -> The message parameters.
 *                                              r specifies whether the message is request (r==0) or response (r==1)
 *                                              c specifies whether the message shall contain the checksum
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
 * Minimum message length (empty payload, disabled checksum): 1b+1b+1b+1b+0b+0b+1b = 5b
 *
 */

__EXTERN_C_BEGIN

const static uint8_t uproto_message_start = 0b00110011;
const static uint8_t uproto_message_end = 0b11001100;

const static uint8_t uproto_message_property_request_response_mask = 0b00000001;
const static uint8_t uproto_message_property_skip_checksum_mask = 0b00000010;

typedef enum {
    uproto_message_status_unknown = 0,
    uproto_message_status_ok,
    uproto_message_status_parsing,
    uproto_message_status_end_invalid,
} uproto_parse_status;

struct uproto_message_t {
    uint8_t message_properties;
    uint32_t resource_id;
    uint32_t payload_length;
    uint8_t* payload;
    uint8_t checksum;

    uproto_parse_status parse_status;
};

/**
 * Creates a new empty instance of uproto message.
 * @returns Pointer to the created instance.
 */
uproto_message_t* uproto_message_create();

/**
 * Destroys the uproto message instance and its associated data.
 * Caution! This will destroy the allocated payload buffer if it was initialized.
 * @param message_ptr Pointer reference to the instance.
 */
void uproto_message_destroy(uproto_message_t** message_ptr);

/**
 * Checks whether the uproto message instance is valid.
 * This will not validate the message itself.
 * @param message The message.
 * @returns Whether the message instance is valid.
 */
bool uproto_message_is_valid(const uproto_message_t* message);

bool uproto_message_is_request(uproto_message_t* message);
bool uproto_message_is_response(uproto_message_t* message);

void uproto_message_set_as_request_type(uproto_message_t* message);
void uproto_message_set_as_response_type(uproto_message_t* message);

bool uproto_message_has_checksum(uproto_message_t* message);
bool uproto_message_skips_checksum(uproto_message_t* message);
uint8_t uproto_message_calculate_checksum(uproto_message_t* message);
bool uproto_message_is_checksum_valid(uproto_message_t* message, uint8_t checksum);

uint8_t* uproto_message_serialize(uproto_message_t* message, size_t* size_ptr);

__EXTERN_C_END

#endif  // UUTILS_UPROTO_MESSAGE_H_
