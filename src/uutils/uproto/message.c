#include "uutils/uproto/message.h"
#include "uutils/checksum.h"

uproto_message_t* uproto_message_create() {
    uproto_message_t* message = malloc(sizeof(uproto_message_t));

    message->message_properties = 0;
    message->resource_id = 0;
    message->payload_length = 0;
    message->payload = NULL;
    message->checksum = 0;
    message->parse_status = uproto_message_status_unknown;

    return message;
}

void uproto_message_destroy(uproto_message_t** message_ptr) {
    if (message_ptr == NULL || *message_ptr == NULL) {
        return;
    }

    if ((*message_ptr)->payload != NULL) {
        free((*message_ptr)->payload);
    }

    free(*message_ptr);
    *message_ptr = NULL;
}

bool uproto_message_is_valid(const uproto_message_t* message) {
    return message != NULL;
}

bool uproto_message_is_request(uproto_message_t* message) {
    return (message->message_properties & uproto_message_property_request_response_mask) == 0;
}

bool uproto_message_is_response(uproto_message_t* message) {
    return (message->message_properties & uproto_message_property_request_response_mask) == uproto_message_property_request_response_mask;
}

void uproto_message_set_as_request_type(uproto_message_t* message) {
    message->message_properties &= ~uproto_message_property_request_response_mask;

}

void uproto_message_set_as_response_type(uproto_message_t* message) {
    message->message_properties |= uproto_message_property_request_response_mask;
}

bool uproto_message_has_checksum(uproto_message_t* message) {
    return (message->message_properties & uproto_message_property_skip_checksum_mask) == 0;
}

bool uproto_message_skips_checksum(uproto_message_t* message) {
    return (message->message_properties & uproto_message_property_skip_checksum_mask) == uproto_message_property_skip_checksum_mask;
}

uint8_t uproto_message_calculate_checksum(uproto_message_t* message) {
    uint8_t checksum = uproto_message_start;
    checksum += message->message_properties;
    checksum += checksum_simple_dynamic_value(message->resource_id);
    checksum += checksum_simple_dynamic_value(message->payload_length);

    if (message->payload_length > 0 && message->payload != NULL) {
        for (size_t i = 0; i < message->payload_length; ++i) {
            checksum += message->payload[i];
        }
    }

    return checksum;
}

bool uproto_message_is_checksum_valid(uproto_message_t* message, uint8_t checksum) {
    return checksum == uproto_message_calculate_checksum(message);
}
