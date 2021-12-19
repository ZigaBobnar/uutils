#include "uutils/dynamic_value.h"

__EXTERN_C_BEGIN

static const dynamic dynamic_invalid_real_value = (dynamic)0xFFFFFFFFFFFFFFFF;

static const uint8_t dynamic_negative_mask = 0b10000000;
static const uint8_t dynamic_1b2b_mask = 0b01000000;
static const uint8_t dynamic_2b4b_mask = 0b00100000;
static const uint8_t dynamic_4b8b_mask = 0b00010000;
static const uint8_t dynamic_8b16b_mask = 0b00001000;
static const uint8_t dynamic_2b_preamble = 0b01000000;
static const uint8_t dynamic_4b_preamble = 0b01100000;
static const uint8_t dynamic_8b_preamble = 0b01110000;
static const uint8_t dynamic_16b_preamble = 0b01111000;

static const dynamic_real dynamic_1b_raw_range = 0x3F;
static const dynamic_real dynamic_1b_max = 0x3F;
static const dynamic_real dynamic_1b_min = -0x3F - 1;
static const int8_t dynamic_1b_min_limited = 0xC0;
static const dynamic dynamic_1b_data_mask = 0x3F;
const uint8_t uint8_t_max_value = 0xFF;

#if DYNAMIC_HAS_2B
static const dynamic_real dynamic_2b_raw_range = 0x1FFF;
static const dynamic_real dynamic_2b_pos_offset = 0x0040;
static const dynamic_real dynamic_2b_neg_offset = (int16_t)0xFFBF;
static const int16_t dynamic_2b_neg_offset_limited = 0xFFBF;
static const dynamic_real dynamic_2b_max = 0x203F;
static const dynamic_real dynamic_2b_min = (int16_t)0xDFC0;
static const int16_t dynamic_2b_min_limited = 0xDFC0;
static const dynamic dynamic_2b_data_mask = 0x1FFF;
const uint16_t uint16_t_max_value = 0xFFFF;
#endif

#if DYNAMIC_HAS_4B
static const dynamic_real dynamic_4b_raw_range = 0x0FFFFFFF;
static const dynamic_real dynamic_4b_pos_offset = 0x00002040;
static const dynamic_real dynamic_4b_neg_offset = (int32_t)0xFFFFDFBF;
static const int32_t dynamic_4b_neg_offset_limited = 0xFFFFDFBF;
static const dynamic_real dynamic_4b_max = 0x1000203F;
static const dynamic_real dynamic_4b_min = (int32_t)0xEFFFDFC0;
static const int32_t dynamic_4b_min_limited = 0xEFFFDFC0;
static const dynamic dynamic_4b_data_mask = 0x0FFFFFFF;
const uint32_t uint32_t_max_value = 0xFFFFFFFF;
#endif

#if DYNAMIC_HAS_8B
static const dynamic_real dynamic_8b_raw_range = 0x07FFFFFFFFFFFFFF;
static const dynamic_real dynamic_8b_pos_offset = 0x0000000010002040;
static const dynamic_real dynamic_8b_neg_offset = (int64_t)0xFFFFFFFFEFFFDFBF;
static const int64_t dynamic_8b_neg_offset_limited = 0xFFFFFFFFEFFFDFBF;
static const dynamic_real dynamic_8b_max = 0x080000001000203F;
static const dynamic_real dynamic_8b_min = (int64_t)0xF7FFFFFFEFFFDFC0;
static const int64_t dynamic_8b_min_limited = 0xF7FFFFFFEFFFDFC0;
static const dynamic dynamic_8b_data_mask = 0x07FFFFFFFFFFFFFF;
const uint64_t uint64_t_max_value = 0xFFFFFFFFFFFFFFFF;
#endif

dynamic_real dynamic_to_real_internal(const dynamic dynamic_value, uint8_t dynamic_size, bool is_negative) {
    if (!is_negative) {
        if (dynamic_size == 1) {
            return (dynamic_value & dynamic_1b_data_mask);
#if DYNAMIC_HAS_2B
        } else if (dynamic_size == 2) {
            return (dynamic_value & dynamic_2b_data_mask) + dynamic_2b_pos_offset;
#endif
#if DYNAMIC_HAS_4B
        } else if (dynamic_size == 4) {
            return (dynamic_value & dynamic_4b_data_mask) + dynamic_4b_pos_offset;
#endif
#if DYNAMIC_HAS_8B
        } else if (dynamic_size == 8) {
            return (dynamic_value & dynamic_8b_data_mask) + dynamic_8b_pos_offset;
#endif
        } else {
            return dynamic_invalid_real_value;
        }
    } else {
        if (dynamic_size == 1) {
            uint8_t value = (uint8_t)(~dynamic_value & dynamic_1b_data_mask);
            return (dynamic_real)(int8_t)(~value);
#if DYNAMIC_HAS_2B
        } else if (dynamic_size == 2) {
            uint16_t value = (uint16_t)(~dynamic_value | ~dynamic_2b_data_mask);
            value = ~value;
            value |= 1 << 15;
            value |= 1 << 14;
            value |= 1 << 13;
            value -= 64;
            return (dynamic_real)((int16_t)(value));
#endif
#if DYNAMIC_HAS_4B
        } else if (dynamic_size == 4) {
            uint32_t value = (uint32_t)(~dynamic_value | ~dynamic_4b_data_mask);
            value = ~value;
            value |= 1 << 31;
            value |= 1 << 30;
            value |= 1 << 29;
            value |= 1 << 28;
            value -= 8256;
            return (dynamic_real)((int32_t)(value));
#endif
#if DYNAMIC_HAS_8B
        } else if (dynamic_size == 8) {
            uint64_t value = (uint64_t)(~dynamic_value | ~dynamic_8b_data_mask);
            value = ~value;
            value |= (uint64_t)1 << 63;
            value |= (uint64_t)1 << 62;
            value |= (uint64_t)1 << 61;
            value |= (uint64_t)1 << 60;
            value |= (uint64_t)1 << 59;
            value -= 268443712;
            return (dynamic_real)(value);
#endif
        } else {
            return dynamic_invalid_real_value;
        }
    }
}

dynamic_real dynamic_to_real(const dynamic dynamic_value, uint8_t* dynamic_size_out) {
    uint8_t dynamic_size =
#if DYNAMIC_HAS_8B
        dynamic_value > uint32_t_max_value ? 8 :
#endif
#if DYNAMIC_HAS_4B
        dynamic_value > uint16_t_max_value ? 4 :
#endif
#if DYNAMIC_HAS_2B
        dynamic_value > uint8_t_max_value ? 2 :
#endif
        1;

    uint8_t preamble_value = (uint8_t)(
#if DYNAMIC_HAS_8B
        dynamic_size == 8 ? dynamic_value >> 56 :
#endif
#if DYNAMIC_HAS_4B
        dynamic_size == 4 ? dynamic_value >> 24 :
#endif
#if DYNAMIC_HAS_2B
        dynamic_size == 2 ? dynamic_value >> 8 :
#endif
        dynamic_value
    );

    bool is_negative = preamble_value & dynamic_negative_mask;
    if (is_negative) {
        preamble_value = ~preamble_value;
    }

    uint8_t encoded_size =
#if DYNAMIC_HAS_16B
        (preamble_value & dynamic_16b_preamble) == dynamic_16b_preamble ? 16 :
#endif
#if DYNAMIC_HAS_8B
        (preamble_value & dynamic_8b_preamble) == dynamic_8b_preamble ? 8 :
#endif
#if DYNAMIC_HAS_4B
        (preamble_value & dynamic_4b_preamble) == dynamic_4b_preamble ? 4 :
#endif
#if DYNAMIC_HAS_2B
        (preamble_value & dynamic_2b_preamble) == dynamic_2b_preamble ? 2 :
#endif
        1;

    if (encoded_size != dynamic_size) {
        return dynamic_invalid_real_value;
    }

    if (dynamic_size_out != NULL) {
        *dynamic_size_out = dynamic_size;
    }

    return dynamic_to_real_internal(dynamic_value, dynamic_size, is_negative);
}

dynamic_real dynamic_buffer_to_real(uint8_t* input, uint8_t** out_ptr) {
    uint8_t preamble_value = *input;
    bool is_negative = preamble_value & dynamic_negative_mask;
    if (is_negative) {
        preamble_value = ~preamble_value;
    }

    uint8_t encoded_size =
#if DYNAMIC_HAS_16B
        (preamble_value & dynamic_16b_preamble) == dynamic_16b_preamble ? 16 :
#endif
#if DYNAMIC_HAS_8B
        (preamble_value & dynamic_8b_preamble) == dynamic_8b_preamble ? 8 :
#endif
#if DYNAMIC_HAS_4B
        (preamble_value & dynamic_4b_preamble) == dynamic_4b_preamble ? 4 :
#endif
#if DYNAMIC_HAS_2B
        (preamble_value & dynamic_2b_preamble) == dynamic_2b_preamble ? 2 :
#endif
        1;

    dynamic dynamic_value = 0;
    if (encoded_size == 1) {
        dynamic_value = *input;
#if DYNAMIC_HAS_2B
    } else if (encoded_size == 2) {
        dynamic_value =
            ((dynamic)(*input) << 8) |
            (*++input);
#endif
#if DYNAMIC_HAS_4B
    } else if (encoded_size == 4) {
        dynamic_value =
            ((dynamic)(*input) << 24) |
            ((dynamic)(*++input) << 16) |
            ((dynamic)(*++input) << 8) |
            (*++input);
#endif
#if DYNAMIC_HAS_8B
    } else if (encoded_size == 8) {
        dynamic_value =
            ((dynamic)(*input) << 56) |
            ((dynamic)(*++input) << 48) |
            ((dynamic)(*++input) << 40) |
            ((dynamic)(*++input) << 32) |
            ((dynamic)(*++input) << 24) |
            ((dynamic)(*++input) << 16) |
            ((dynamic)(*++input) << 8) |
            (*++input);
#endif
    } else {
        return dynamic_invalid_real_value;
    }

    if (out_ptr != NULL) {
        *out_ptr = input;
    }

    return dynamic_to_real_internal(dynamic_value, encoded_size, is_negative);
}

uint8_t dynamic_to_real_get_required_bytes(const uint8_t first_byte_input) {
    uint8_t preamble = (first_byte_input & dynamic_negative_mask) ? ~first_byte_input : first_byte_input;

    return
#if DYNAMIC_HAS_16B
        (preamble & dynamic_16b_preamble) == dynamic_16b_preamble ? 16 :
#endif
#if DYNAMIC_HAS_8B
        (preamble & dynamic_8b_preamble) == dynamic_8b_preamble ? 8 :
#endif
#if DYNAMIC_HAS_4B
        (preamble & dynamic_4b_preamble) == dynamic_4b_preamble ? 4 :
#endif
#if DYNAMIC_HAS_2B
        (preamble & dynamic_2b_preamble) == dynamic_2b_preamble ? 2 :
#endif
        1;
}

uint8_t dynamic_find_preamble_byte(const dynamic dynamic_value) {
    uint8_t dynamic_size =
#if DYNAMIC_HAS_8B
        dynamic_value > uint32_t_max_value ? 8 :
#endif
#if DYNAMIC_HAS_4B
        dynamic_value > uint16_t_max_value ? 4 :
#endif
#if DYNAMIC_HAS_2B
        dynamic_value > uint8_t_max_value ? 2 :
#endif
        1;

    uint8_t preamble_value = (uint8_t)(
#if DYNAMIC_HAS_8B
        dynamic_size == 8 ? dynamic_value >> 56 :
#endif
#if DYNAMIC_HAS_4B
        dynamic_size == 4 ? dynamic_value >> 24 :
#endif
#if DYNAMIC_HAS_2B
        dynamic_size == 2 ? dynamic_value >> 8 :
#endif
        dynamic_value
    );

    return preamble_value;
}

dynamic real_to_dynamic(const dynamic_real value, uint8_t* written_bytes) {
    uint8_t dynamic_size = real_to_dynamic_get_required_bytes(value);

    if (written_bytes != NULL) {
        *written_bytes = dynamic_size;
    }

    if (value >= 0) {
        if (dynamic_size == 1) {
            return value;
#if DYNAMIC_HAS_2B
        } else if (dynamic_size == 2) {
            dynamic a = (dynamic)(value - dynamic_2b_pos_offset);
            a &= dynamic_2b_data_mask;
            a |= ((dynamic)dynamic_2b_preamble) << 8;
            return a;
#endif
#if DYNAMIC_HAS_4B
        } else if (dynamic_size == 4) {
            dynamic a = (dynamic)(value - dynamic_4b_pos_offset);
            a &= dynamic_4b_data_mask;
            a |= ((dynamic)dynamic_4b_preamble) << 24;
            return a;
#endif
#if DYNAMIC_HAS_8B
        }  else if (dynamic_size == 8) {
            dynamic a = (dynamic)(value - dynamic_8b_pos_offset);
            a &= dynamic_8b_data_mask;
            a |= ((dynamic)dynamic_8b_preamble) << 56;
            return a;
#endif
        } else {
            return 0;
        }
    } else {
        if (dynamic_size == 1) {
            return (uint8_t)value;
#if DYNAMIC_HAS_2B
        } else if (dynamic_size == 2) {
            uint16_t a = (uint16_t)(value - dynamic_2b_neg_offset - 1);
            a = ~a;
            a &= dynamic_2b_data_mask;
            a |= ((uint16_t)dynamic_2b_preamble) << 8;
            a = ~a;
            return a;
#endif
#if DYNAMIC_HAS_4B
        } else if (dynamic_size == 4) {
            uint32_t a = (uint32_t)(value - dynamic_4b_neg_offset - 1);
            a = ~a;
            a &= dynamic_4b_data_mask;
            a |= ((uint32_t)dynamic_4b_preamble) << 24;
            a = ~a;
            return a;
#endif
#if DYNAMIC_HAS_8B
        }  else if (dynamic_size == 8) {
            uint64_t a = (uint64_t)(value - dynamic_8b_neg_offset - 1);
            a = ~a;
            a &= dynamic_8b_data_mask;
            a |= ((uint64_t)dynamic_8b_preamble) << 56;
            a = ~a;
            return a;
#endif
        } else {
            return 0;
        }
    }
}

uint8_t real_to_dynamic_get_required_bytes(const dynamic_real value) {
#if DYNAMIC_HAS_16B
    if (value > dynamic_8b_max || value < dynamic_8b_min) {
        return 16;
    } else
#endif
#if DYNAMIC_HAS_8B
    if (value > dynamic_4b_max || value < dynamic_4b_min) {
        return 8;
    } else
#endif
#if DYNAMIC_HAS_4B
if (value > dynamic_2b_max || value < dynamic_2b_min) {
        return 4;
    } else
#endif
#if DYNAMIC_HAS_2B
    if (value > dynamic_1b_max || value < dynamic_1b_min) {
        return 2;
    } else
#endif
    {
        return 1;
    }
}

uint8_t real_to_dynamic_buffer(dynamic_real value, uint8_t* buffer) {
    uint8_t written_bytes;
    dynamic serialized = real_to_dynamic(value, &written_bytes);

#if DYNAMIC_HAS_8B
    if (written_bytes == 8) {
        buffer[0] = (uint8_t)(serialized >> 56);
        buffer[1] = (uint8_t)(serialized >> 48);
        buffer[2] = (uint8_t)(serialized >> 40);
        buffer[3] = (uint8_t)(serialized >> 32);
        buffer[4] = (uint8_t)(serialized >> 24);
        buffer[5] = (uint8_t)(serialized >> 16);
        buffer[6] = (uint8_t)(serialized >> 8);
        buffer[7] = (uint8_t)(serialized);
    } else
#endif
#if DYNAMIC_HAS_4B
    if (written_bytes == 4) {
        buffer[0] = (uint8_t)(serialized >> 24);
        buffer[1] = (uint8_t)(serialized >> 16);
        buffer[2] = (uint8_t)(serialized >> 8);
        buffer[3] = (uint8_t)(serialized);
    } else
#endif
#if DYNAMIC_HAS_2B
    if (written_bytes == 2) {
        buffer[0] = (uint8_t)(serialized >> 8);
        buffer[1] = (uint8_t)(serialized);
    } else
#endif
    if (written_bytes == 1) {
        buffer[0] = (uint8_t)(serialized);
    }

    return written_bytes;
}

bool dynamic_to_real_stateful(dynamic_state_t* state, uint8_t value, dynamic_real* return_ptr) {
    if (state->required_num == 0) {
        state->required_num = dynamic_to_real_get_required_bytes(value);
        state->cached_num = 0;
    }

    if (state->required_num == 1) {
        *return_ptr = dynamic_to_real(value, NULL);
        state->required_num = 0;

        return true;
    }

    state->cache[state->cached_num++] = value;

    if (state->cached_num == state->required_num) {
        *return_ptr = dynamic_buffer_to_real(state->cache, NULL);
        state->required_num = 0;

        return true;
    }

    return false;
}

__EXTERN_C_END
