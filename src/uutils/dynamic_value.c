#include "uutils/dynamic_value.h"

__EXTERN_C_BEGIN

static const uint64_t preamble_2b_mode = ((uint64_t)1) << 15;
static const uint64_t preamble_4b_mode = ((uint64_t)1) << 31 | ((uint64_t)1) << 30;
static const uint64_t preamble_8b_mode = ((uint64_t)1) << 63 | ((uint64_t)1) << 62 | ((uint64_t)1) << 61;

static const uint64_t offset_2b_mode = ((uint64_t)0x80);
static const uint64_t offset_4b_mode = ((uint64_t)0x4080);
static const uint64_t offset_8b_mode = ((uint64_t)0x20004080);

uint64_t dynamic_parse(const uint64_t input, uint8_t* dynamic_size_out) {
    uint8_t dynamic_size = ((input & preamble_8b_mode) == preamble_8b_mode) ? 8 : 
        ((input & preamble_4b_mode) == preamble_4b_mode) ? 4 :
        ((input & preamble_2b_mode) == preamble_2b_mode) ? 2 :
        1;

    if (dynamic_size_out != NULL) {
        *dynamic_size_out = dynamic_size;
    }

    return dynamic_size == 1 ? input :
        dynamic_size == 2 ? (input ^ preamble_2b_mode) + offset_2b_mode :
        dynamic_size == 4 ? (input ^ preamble_4b_mode) + offset_4b_mode :
        dynamic_size == 8 ? (input ^ preamble_8b_mode) + offset_8b_mode :
        0;
}

uint64_t dynamic_parse_buffer(uint8_t* input, uint8_t** out_ptr) {
    uint64_t value = *input;

    uint8_t* pos = input;

    if ((value & 0xE0) == 0xE0) {
        value = offset_8b_mode + (
            ((uint64_t)value) << 56 |
            ((uint64_t)(*++pos)) << 48 |
            ((uint64_t)(*++pos)) << 40 |
            ((uint64_t)(*++pos)) << 32 |
            ((uint64_t)(*++pos)) << 24 |
            ((uint64_t)(*++pos)) << 16 |
            ((uint32_t)(*++pos)) << 8 |
            *++pos)
            ^ preamble_8b_mode;
    } else if ((value & 0xC0) == 0xC0) {
        value = offset_4b_mode + (
            ((uint64_t)value) << 24 |
            ((uint64_t)(*++pos)) << 16 |
            ((uint32_t)(*++pos)) << 8 |
            *++pos)
            ^ preamble_4b_mode;
    } else if ((value & 0x80) == 0x80) {
        value = offset_2b_mode + (
            ((uint32_t)value) << 8 |
            *++pos)
            ^ preamble_2b_mode;
    }

    if (out_ptr != NULL) {
        *out_ptr = pos;
    }

    return value;
}

uint8_t dynamic_parse_get_required_bytes(const uint8_t first_byte_input) {
    return (first_byte_input & 0b11100000) == 0b11100000 ? 8 :
        (first_byte_input & 0b11000000) == 0b11000000 ? 4 :
        (first_byte_input & 0b10000000) == 0b10000000 ? 2 :
        1;
}

uint64_t dynamic_serialize(const uint64_t value, uint8_t* written_bytes) {
    uint8_t dynamic_size = dynamic_serialize_get_required_bytes(value);
    if (written_bytes != NULL) {
        *written_bytes = dynamic_size;
    }

    if (dynamic_size == 1) {
        return value;
    } else if (dynamic_size == 2) {
        return (preamble_2b_mode | value) - offset_2b_mode;
    } else if (dynamic_size == 4) {
        return (preamble_4b_mode | value) - offset_4b_mode;
    } else if (dynamic_size == 8) {
        return (preamble_8b_mode | value) - offset_8b_mode;
    }

    return 0;
}

uint8_t dynamic_serialize_get_required_bytes(const uint64_t value) {
    return (value >= offset_8b_mode) ? 8 :
        (value >= offset_4b_mode) ? 4 :
        (value >= offset_2b_mode) ? 2 :
        1;
}

uint8_t dynamic_serialize_into_buffer(uint64_t value, uint8_t* buffer) {
    uint8_t written_bytes;
    uint64_t serialized = dynamic_serialize(value, &written_bytes);
    if (written_bytes == 8) {
        buffer[0] = (uint8_t)(serialized >> 56);
        buffer[1] = (uint8_t)(serialized >> 48);
        buffer[2] = (uint8_t)(serialized >> 40);
        buffer[3] = (uint8_t)(serialized >> 32);
        buffer[4] = (uint8_t)(serialized >> 24);
        buffer[5] = (uint8_t)(serialized >> 16);
        buffer[6] = (uint8_t)(serialized >> 8);
        buffer[7] = (uint8_t)(serialized);
    } else if (written_bytes == 4) {
        buffer[0] = (uint8_t)(serialized >> 24);
        buffer[1] = (uint8_t)(serialized >> 16);
        buffer[2] = (uint8_t)(serialized >> 8);
        buffer[3] = (uint8_t)(serialized);
    } else if (written_bytes == 2) {
        buffer[0] = (uint8_t)(serialized >> 8);
        buffer[1] = (uint8_t)(serialized);
    } else if (written_bytes == 1) {
        buffer[0] = (uint8_t)(serialized);
    }

    return written_bytes;
}

__EXTERN_C_END
