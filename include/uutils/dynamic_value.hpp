#ifndef UUTILS_DYNAMIC_VALUE_HPP_
#define UUTILS_DYNAMIC_VALUE_HPP_

#include "dynamic_value.h"

#include <vector>
#include <tuple>

/**
 * Parses the dynamic value from input vector.
 * @param input Input vector.
 * @returns Tuple that contains parsed value and the required dynamic serialized length used by this value.
 */
std::tuple<uint64_t, uint8_t> dynamic_parse(const std::vector<uint8_t>& input);

/**
 * Converts the value into dynamic value. The result is vector of bytes that can be parsed again.
 * @param value The input value to convert.
 * @returns The serialized value.
 */
std::vector<uint8_t> dynamic_serialize_to_vector(const uint64_t value);

/**
 * Splits the 64-bit value into appropriately sized vector of bytes.
 * @param value The input value to split.
 * @returns The split value.
 */
std::vector<uint8_t> dynamic_split_to_bytes(const uint64_t value);

#endif  // UUTILS_DYNAMIC_VALUE_HPP_
