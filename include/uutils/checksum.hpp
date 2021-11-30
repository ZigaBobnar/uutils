#ifndef UUTILS_CHECKSUM_HPP_
#define UUTILS_CHECKSUM_HPP_

#include "checksum.h"

#include <vector>

/**
 * Calculates checksum of provided data.
 * Simple checksum is calculated as a modulus of sum of all bytes in data with 256 (uint8_t cast).
 * @param data Data vector.
 * @returns The checksum byte.
 */
const uint8_t checksum_simple(std::vector<uint8_t> data);

/**
 * Calculates the new checksum and add existing checksum value.
 * @param previous_checksum Existing checksum.
 * @param data Data vector.
 * @returns The combined checksum.
 */
const uint8_t checksum_simple_combine(uint8_t previous_checksum, std::vector<uint8_t> data);

#endif  // UUTILS_CHECKSUM_HPP_
