#include "uutils/uproto/version.h"
#include "uutils/dynamic_value.h"

uproto_version_info_t uproto_version_info_parse(uint8_t* buffer, size_t max_length) {
    uproto_version_info_t info = { 0, 0, 0 };

    uint8_t* buffer_pos = buffer;
    info.major = (uint32_t)dynamic_parse_buffer(buffer_pos, &buffer_pos);
    info.minor = (uint32_t)dynamic_parse_buffer(buffer_pos, &buffer_pos);
    info.patch = (uint32_t)dynamic_parse_buffer(buffer_pos, &buffer_pos);

    return info;
}

uint8_t* uproto_version_info_serialize(uproto_version_info_t* info, uint32_t* length) {
    uint32_t total_length = dynamic_serialize_get_required_bytes(info->major) +
        dynamic_serialize_get_required_bytes(info->minor) +
        dynamic_serialize_get_required_bytes(info->patch);

    if (length != NULL) {
        *length = total_length;
    }
    uint8_t* buffer = malloc(total_length);

    uint8_t* buffer_pos = buffer;
    buffer_pos += dynamic_serialize_into_buffer(info->major, buffer_pos);
    buffer_pos += dynamic_serialize_into_buffer(info->minor, buffer_pos);
    dynamic_serialize_into_buffer(info->patch, buffer_pos);

    return buffer;
}
