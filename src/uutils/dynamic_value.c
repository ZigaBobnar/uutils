#include "uutils/dynamic_value.h"

__EXTERN_C_BEGIN

static const uint64_t preamble_4b_mode = ((uint64_t)1) << 31 | ((uint64_t)1) << 30;
static const uint64_t preamble_2b_mode = ((uint64_t)1) << 15;

static const uint64_t offset_2b_mode = ((uint64_t)0x80);
static const uint64_t offset_4b_mode = ((uint64_t)0x4080);

uint64_t dynamic_parse(uint64_t input, uint8_t* dynamic_size_out) {
    uint8_t dynamic_size = ((input & preamble_4b_mode) == preamble_4b_mode) ? 4 :
        ((input & preamble_2b_mode) == preamble_2b_mode) ? 2 :
        1;

    if (dynamic_size_out != NULL) {
        *dynamic_size_out = dynamic_size;
    }

    return dynamic_size == 1 ? input :
        dynamic_size == 2 ? (input ^ preamble_2b_mode) + offset_2b_mode :
        dynamic_size == 4 ? (input ^ preamble_4b_mode) + offset_4b_mode :
        0;
}

uint64_t dynamic_parse_buffer(uint8_t* input, uint8_t** out_ptr) {
    uint64_t value = *input;

    uint8_t* pos = input;

    if ((value & 0xC0) == 0xC0) {
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

uint8_t dynamic_parse_get_required_bytes(uint8_t first_byte_input) {
    return (first_byte_input & 0b11100000) == 0b11100000 ? 8 :
        (first_byte_input & 0b11000000) == 0b11000000 ? 4 :
        (first_byte_input & 0b10000000) == 0b10000000 ? 2 :
        1;
}

uint64_t dynamic_serialize(uint64_t value, uint8_t* written_bytes) {
    uint8_t dynamic_size = dynamic_serialize_get_required_bytes(value);
    if (written_bytes != NULL) {
        *written_bytes = dynamic_size;
    }

    if (dynamic_size == 1) {
        return value;
    } else if (dynamic_size == 2) {
        return preamble_2b_mode |
            value - offset_2b_mode;
    } else if (dynamic_size == 4) {
        return preamble_4b_mode |
            value - offset_4b_mode;
    }

    return 0;
}

uint8_t dynamic_serialize_get_required_bytes(uint64_t value) {
    return (value > 0x2000407F) ? 8 :
        (value >= offset_4b_mode) ? 4 :
        (value >= offset_2b_mode) ? 2 :
        1;
}

__EXTERN_C_END
