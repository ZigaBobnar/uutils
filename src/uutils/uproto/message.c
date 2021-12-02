#include "uutils/uproto/message.h"

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
