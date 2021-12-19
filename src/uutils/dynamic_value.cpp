#include "uutils/dynamic_value.hpp"

std::tuple<dynamic_real, uint8_t> dynamic_to_real(std::vector<uint8_t>& dynamic_value) {
    uint8_t* output_ptr;

    dynamic_real result = dynamic_buffer_to_real(&dynamic_value[0], &output_ptr);
    uint8_t size = (uint8_t)(output_ptr - dynamic_value.data() + 1);

    return std::make_tuple(result, size);
}

std::vector<uint8_t> real_to_dynamic_vector(const dynamic_real value) {
    uint8_t written_bytes;

    dynamic_real result = real_to_dynamic(value, &written_bytes);

    std::vector<uint8_t> vector_result = std::vector<uint8_t>();
    vector_result.reserve(written_bytes);
    for (int8_t i = written_bytes - 1; i >= 0; --i) {
        vector_result.push_back((uint8_t)(result >> (i * 8)));
    }

    return vector_result;
}

std::vector<uint8_t> dynamic_split_to_bytes(const dynamic dynamic_value) {
    uint8_t preamble = dynamic_find_preamble_byte(dynamic_value);

    uint8_t size = dynamic_to_real_get_required_bytes(preamble);
#if DYNAMIC_HAS_8B
    if (size == 8) {
        return {
            (uint8_t)(dynamic_value >> 56),
            (uint8_t)(dynamic_value >> 48),
            (uint8_t)(dynamic_value >> 40),
            (uint8_t)(dynamic_value >> 32),
            (uint8_t)(dynamic_value >> 24),
            (uint8_t)(dynamic_value >> 16),
            (uint8_t)(dynamic_value >> 8),
            (uint8_t)(dynamic_value),
        };
    } else
#endif
#if DYNAMIC_HAS_4B
    if (size == 4) {
        return {
            (uint8_t)(dynamic_value >> 24),
            (uint8_t)(dynamic_value >> 16),
            (uint8_t)(dynamic_value >> 8),
            (uint8_t)(dynamic_value),
        };
    } else
#endif
#if DYNAMIC_HAS_2B
    if (size == 2) {
        return {
            (uint8_t)(dynamic_value >> 8),
            (uint8_t)(dynamic_value),
        };
    } else
#endif
    if (size == 1) {
        return {
            (uint8_t)(dynamic_value),
        };
    } else {
        return {};
    }
}
