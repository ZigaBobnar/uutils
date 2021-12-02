#include "uutils/uproto/uproto.h"

uproto_message_t* uproto_message_create() {
    uproto_message_t* message = malloc(sizeof(uproto_message_t));

    message->message_properties = 0;
    message->resource_id = 0;
    message->payload_length = 0;
    message->payload = NULL;
    message->checksum = 0;
    message->parse_status = uproto_message_unknown;

    return message;
}
