#include "uutils/checksum.hpp"

const uint8_t checksum_simple(std::vector<uint8_t> data) {
    uint8_t sum = 0;

    for (const auto& value : data) {
        sum += value;
    }

    return sum;
}

const uint8_t checksum_simple_combine(uint8_t previous_checksum, std::vector<uint8_t> data) {
    return previous_checksum + checksum_simple(data);
}
