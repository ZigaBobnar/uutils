#include "uutils/checksum.h"
#include "uutils/dynamic_value.h"

__EXTERN_C_BEGIN

const uint8_t checksum_simple(uint8_t* data, const size_t length) {
    uint8_t sum = 0;
    uint8_t* pos = data;
    uint8_t* end = data + length - 1;

    while (pos <= end) {
        sum += *(pos++);
    }

    return sum;
}

const uint8_t checksum_simple_combine(uint8_t previous_checksum, uint8_t* data, size_t length) {
    return previous_checksum + checksum_simple(data, length);
}

const uint8_t checksum_simple_dynamic_value(dynamic_real real_value) {
    uint8_t dynamic_size;
    int64_t serialized_value = real_to_dynamic(real_value, &dynamic_size);

    return dynamic_size == 1 ? (uint8_t)serialized_value :
#if DYNAMIC_HAS_2B
        dynamic_size == 2 ? (uint8_t)(serialized_value >> 8) + (uint8_t)(serialized_value) :
#endif
#if DYNAMIC_HAS_4B
        dynamic_size == 4 ? (uint8_t)(serialized_value >> 24) + (uint8_t)(serialized_value >> 16) + (uint8_t)(serialized_value >> 8) + (uint8_t)(serialized_value) :
#endif
#if DYNAMIC_HAS_8B
        dynamic_size == 8 ? (uint8_t)(serialized_value >> 56) + (uint8_t)(serialized_value >> 48) + (uint8_t)(serialized_value >> 40) + (uint8_t)(serialized_value >> 32) + (uint8_t)(serialized_value >> 24) + (uint8_t)(serialized_value >> 16) + (uint8_t)(serialized_value >> 8) + (uint8_t)(serialized_value) :
#endif
        0;
}

__EXTERN_C_END
