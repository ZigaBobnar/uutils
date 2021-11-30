#include "uutils/dynamic_value.hpp"

std::tuple<uint64_t, uint8_t> dynamic_parse(const std::vector<uint8_t>& input) {
    uint8_t* output_ptr;

    uint64_t result = dynamic_parse_buffer(input.data(), &output_ptr);
    
    return std::make_tuple(std::move(result), output_ptr - input.data() + 1);
}

std::vector<uint8_t> dynamic_serialize_to_vector(const uint64_t value) {
    uint8_t written_bytes;

    uint64_t result = dynamic_serialize(value, &written_bytes);

    std::vector<uint8_t> vector_result = std::vector<uint8_t>();
    vector_result.reserve(written_bytes);
    for (int8_t i = written_bytes - 1; i >= 0; --i) {
        vector_result.push_back((uint8_t)(result >> (i * 8)));
    }

    return vector_result;
}

std::vector<uint8_t> dynamic_split_to_bytes(const uint64_t value) {
    if (value >= 0x10000) {
        return {
            (uint8_t)(value >> 24),
            (uint8_t)(value >> 16),
            (uint8_t)(value >> 8),
            (uint8_t)(value),
        };
    } else if (value >= 0x100) {
        return {
            (uint8_t)(value >> 8),
            (uint8_t)(value),
        };
    } else {
        return {
            (uint8_t)(value),
        };
    }
}
