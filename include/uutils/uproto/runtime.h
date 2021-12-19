#ifndef UUTILS_UPROTO_RUNTIME_H_
#define UUTILS_UPROTO_RUNTIME_H_

#include "uutils/uproto/core.h"
#include "uutils/uproto/version.h"

#define UPROTO_RUNTIME_MAX_RECEIVE_ADAPTERS 8
#define UPROTO_RUNTIME_MAX_RESPOND_ADAPTERS 2

__EXTERN_C_BEGIN

const static uint8_t uproto_resource_id_do_nothing = 0x00;
const static uint8_t uproto_resource_id_echo_message = 0x01;
const static uint8_t uproto_resource_id_system_initialize = 0x02;
const static uint8_t uproto_resource_id_system_status = 0x03;
const static uint8_t uproto_resource_id_protocol_version = 0x04;
const static uint8_t uproto_resource_id_application_version = 0x05;
const static uint8_t uproto_resource_id_device_identifier_string = 0x06;
const static uint8_t uproto_resource_id_application_identifier_string = 0x07;

typedef bool (*uproto_message_adapter_t)(uproto_runtime_t*, uproto_message_t*);

struct uproto_runtime_t {
    char* device_identifier;
    char* application_identifier;
    uproto_version_info_t application_version;

    uproto_parser_t* parser;
    uproto_message_adapter_t receive_adapters[UPROTO_RUNTIME_MAX_RECEIVE_ADAPTERS];
    uproto_message_adapter_t respond_adapters[UPROTO_RUNTIME_MAX_RESPOND_ADAPTERS];
};

uproto_runtime_t* uproto_runtime_create();

void uproto_runtime_destroy(uproto_runtime_t** runtime_ptr);

void uproto_runtime_feed_data(uproto_runtime_t* runtime, const uint8_t value);
void uproto_runtime_feed_message(uproto_runtime_t* runtime, uproto_message_t* message);

void uproto_runtime_respond_with_message(uproto_runtime_t* runtime, uproto_message_t* message);

void uproto_runtime_attach_receive_adapter(uproto_runtime_t* runtime, uproto_message_adapter_t adapter);
void uproto_runtime_remove_receive_adapter(uproto_runtime_t* runtime, uproto_message_adapter_t adapter);

void uproto_runtime_attach_respond_adapter(uproto_runtime_t* runtime, uproto_message_adapter_t adapter);
void uproto_runtime_remove_respond_adapter(uproto_runtime_t* runtime, uproto_message_adapter_t adapter);

bool uproto_runtime_system_receive_adapter_action(uproto_runtime_t* runtime, uproto_message_t* message);
bool uproto_runtime_global_console_receive_adapter_action(uproto_runtime_t* runtime, uproto_message_t* message);
bool uproto_runtime_global_console_respond_adapter_action(uproto_runtime_t* runtime, uproto_message_t* message);

__EXTERN_C_END

#endif  // UUTILS_UPROTO_RUNTIME_H_
