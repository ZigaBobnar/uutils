#include "uutils/uproto/version.h"
#include "uutils/dynamic_value.h"

uproto_version_info_t uproto_version_info_parse(uint8_t* buffer, size_t max_length) {
    uproto_version_info_t info = { 0, 0, 0 };

    uint8_t* buffer_pos = buffer;
    info.major = dynamic_buffer_to_real(buffer_pos, &buffer_pos);
    info.minor = dynamic_buffer_to_real(buffer_pos, &buffer_pos);
    info.patch = dynamic_buffer_to_real(buffer_pos, &buffer_pos);

    return info;
}

uint8_t* uproto_version_info_serialize(uproto_version_info_t* info, dynamic_real* length) {
    dynamic_real total_length =
        real_to_dynamic_get_required_bytes(info->major) +
        real_to_dynamic_get_required_bytes(info->minor) +
        real_to_dynamic_get_required_bytes(info->patch);

    if (length != NULL) {
        *length = total_length;
    }
    uint8_t* buffer = malloc(total_length);

    uint8_t* buffer_pos = buffer;
    buffer_pos += real_to_dynamic_buffer(info->major, buffer_pos);
    buffer_pos += real_to_dynamic_buffer(info->minor, buffer_pos);
    real_to_dynamic_buffer(info->patch, buffer_pos);

    return buffer;
}
