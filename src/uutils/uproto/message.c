#include "uutils/uproto/message.h"
#include "uutils/checksum.h"
#include "uutils/dynamic_value.h"

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

uproto_message_t* uproto_message_create_response_for_request(uproto_message_t* request) {
    uproto_message_t* response = uproto_message_create();

    response->message_properties = request->message_properties;
    uproto_message_set_as_response_type(response);
    response->resource_id = request->resource_id;

    return response;
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
    return (message->message_properties & uproto_message_property_has_checksum_mask) == uproto_message_property_has_checksum_mask;
}

bool uproto_message_skips_checksum(uproto_message_t* message) {
    return (message->message_properties & uproto_message_property_has_checksum_mask) == 0;
}

uint8_t uproto_message_calculate_checksum(uproto_message_t* message) {
    uint8_t checksum = uproto_message_start;
    checksum += checksum_simple_dynamic_value(message->message_properties);
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

uint8_t* uproto_message_serialize(uproto_message_t* message, size_t* size_ptr) {
    size_t message_length = 1 +
        real_to_dynamic_get_required_bytes(message->message_properties) +
        real_to_dynamic_get_required_bytes(message->resource_id) +
        real_to_dynamic_get_required_bytes(message->payload_length) +
        message->payload_length +
        (uproto_message_has_checksum(message) ? 1 : 0) +
        1;
    if (size_ptr != NULL) {
        *size_ptr = message_length;
    }

    uint8_t* buffer = malloc(message_length);
    uint8_t* buffer_pos = buffer;

    *buffer_pos++ = uproto_message_start;

    buffer_pos += real_to_dynamic_buffer(message->message_properties, buffer_pos);
    buffer_pos += real_to_dynamic_buffer(message->resource_id, buffer_pos);
    buffer_pos += real_to_dynamic_buffer(message->payload_length, buffer_pos);

    if (message->payload_length > 0) {
        for (size_t i = 0; i < message->payload_length; ++i) {
            *buffer_pos++ = message->payload[i];
        }
    }

    if (uproto_message_has_checksum(message)) {
        *buffer_pos++ = checksum_simple(buffer, buffer_pos - buffer);
    }

    *buffer_pos++ = uproto_message_end;

    if (buffer_pos - buffer != message_length) {
        // Oh no.
    }

    return buffer;
}
