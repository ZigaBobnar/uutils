#ifndef UUTILS_CHECKSUM_H_
#define UUTILS_CHECKSUM_H_

#include "common.h"

__EXTERN_C_BEGIN

/**
 * Calculates checksum of provided data.
 * Simple checksum is calculated as a modulus of sum of all bytes in data with 256 (uint8_t cast).
 * @param data Data buffer.
 * @param length The length of data buffer.
 * @returns The checksum byte.
 */
const uint8_t checksum_simple(uint8_t* data, size_t length);

/**
 * Calculates the new checksum and add existing checksum value.
 * @param previous_checksum Existing checksum.
 * @param data Data buffer.
 * @param length The length of data buffer.
 * @returns The combined checksum.
 */
const uint8_t checksum_simple_combine(uint8_t previous_checksum, uint8_t* data, size_t length);

__EXTERN_C_END

#endif  // UUTILS_CHECKSUM_H_
