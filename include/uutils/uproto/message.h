#ifndef UUTILS_UPROTO_MESSAGE_H_
#define UUTILS_UPROTO_MESSAGE_H_

#include "uutils/uproto/core.h"
#include "uutils/dynamic_value.h"

/**
 * uproto message structure:
 *
 * [1b] message_start = 0b00110011          -> uproto message start and type designator
 *                                              If the message start is not recognized, it will be discarded.
 *
 * [dynamic] message_params = 0b00000ncr    -> The message parameters.
 *                                              r specifies whether the message is request (r==0) or response (r==1)
 *                                              c specifies whether the message shall contain the checksum
 *                                              n specifies whether the networking should be used
 *
 * [dynamic] device_id                      -> The device ID in the network
 *                                              Id 0 is considered as the main controller device (has the ownership over the network)
 *                                              If networking is not used this is skipped
 * 
 * [dynamic] sender_id                      -> The device ID of the sender
 *                                              If networking is not used this is skipped
 * 
 * [dynamic] resource_id                    -> The resource ID
 *                                              Range 0x00-0x1F (0-31) is reserved for system use
 *                                              Range 0x8000-0xFFFF (32768-65535) is also reserved for system use
 *                                              All other resource IDs are available to application usage.
 *
 * [dynamic] payload_length                 -> Total payload length in bytes.
 *
 * [payload_length] payload                 -> The payload. Payload can have its own underlying structure that is processed in handler.
 *
 * [1b?] checksum                           -> Checksum of complete message - if not valid the message is marked as invalid and to be discarded.
 *                                              Calculated as simple sum of complete message and taken the lower 8 bits. Elements present in checksum:
 *                                                  message_start + message_params + resource_id + payload_length + payload + checksum
 *                                              If checksum parameter in message_params is not set the checksum is not included in the message.
 *
 * [1b] message_end = 0b11001100            -> uproto message end.
 *
 * Minimum message length (empty payload, disabled checksum): 1b+1b+1b+1b+0b+0b+1b = 5b
 *
 * System resource ids:
 * - 0x00 => Do nothing (wait for the message end)
 * - 0x01 => Echo message (decode the message and respond with re-encoded message back, useful for testing)
 * - 0x02 => System initialize (initializes the protocol and device core functions)
 * - 0x03 => System status (health report as well as important messages)
 * - 0x04 => Protocol version (used to determine protocol compatibility)
 * - 0x05 => Application version (can be used by applications to see if its definitions are compatible with the device)
 * 
 */

__EXTERN_C_BEGIN

const static uint8_t uproto_message_start = 0b00110011;
const static uint8_t uproto_message_end = 0b11001100;

const static uint8_t uproto_message_property_request_response_mask = 0b00000001;
const static uint8_t uproto_message_property_has_checksum_mask = 0b00000010;
const static uint8_t uproto_message_property_use_networking_mask = 0b00000100;

typedef enum {
    uproto_message_status_unknown = 0,
    uproto_message_status_ok,
    uproto_message_status_parsing,
    uproto_message_status_end_invalid,
} uproto_parse_status;

struct uproto_message_t {
    dynamic_real message_properties;
    dynamic_real resource_id;
#if UPROTO_ENABLE_NETWORKING
    dynamic_real device_id;
    dynamic_real sender_id;
#endif
    dynamic_real payload_length;
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

uproto_message_t* uproto_message_create_response_for_request(uproto_message_t* request);

/**
 * Checks whether the uproto message instance is valid.
 * This will not validate the message itself.
 * @param message The message.
 * @returns Whether the message instance is valid.
 */
bool uproto_message_is_valid(const uproto_message_t* message);

bool uproto_message_is_request(uproto_message_t* message);
bool uproto_message_is_response(uproto_message_t* message);
bool uproto_message_is_using_networking(uproto_message_t* message);

void uproto_message_set_as_request_type(uproto_message_t* message);
void uproto_message_set_as_response_type(uproto_message_t* message);

bool uproto_message_has_checksum(uproto_message_t* message);
bool uproto_message_skips_checksum(uproto_message_t* message);
uint8_t uproto_message_calculate_checksum(uproto_message_t* message);
bool uproto_message_is_checksum_valid(uproto_message_t* message, uint8_t checksum);

uint8_t* uproto_message_serialize(uproto_message_t* message, size_t* size_ptr);

__EXTERN_C_END

#endif  // UUTILS_UPROTO_MESSAGE_H_
