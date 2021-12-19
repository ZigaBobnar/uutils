#ifndef UUTILS_UPROTO_VERSION_H_
#define UUTILS_UPROTO_VERSION_H_

#include "uutils/uproto/core.h"
#include "uutils/dynamic_value.h"

const static uint8_t uproto_protocol_version_major = 0x00;
const static uint8_t uproto_protocol_version_minor = 0x01;
const static uint8_t uproto_protocol_version_patch = 0x00;

struct uproto_version_info_t {
    dynamic_real major;
    dynamic_real minor;
    dynamic_real patch;
};

__EXTERN_C_BEGIN

uproto_version_info_t uproto_version_info_parse(uint8_t* buffer, size_t max_length);
uint8_t* uproto_version_info_serialize(uproto_version_info_t* info, dynamic_real* length);

__EXTERN_C_END

#endif  // UUTILS_UPROTO_VERSION_H_
