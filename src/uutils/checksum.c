#include "uutils/checksum.h"

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

__EXTERN_C_END
