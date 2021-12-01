#ifndef UUTILS_DYNAMIC_VALUE_H_
#define UUTILS_DYNAMIC_VALUE_H_

#include "common.h"

/**
 * Extensible numbers system
 *
 * Those types are used when there is a possibility to start with smaller sized data type, but it might require to grow in length.
 * The notation for data size is as such: [1b,2b,4b,8b], meaning data can grow from simple 8-bit (1 byte) value all the way to 64-bits (8 bytes).
 * In reality the value notation is a bit misleading as in 8-bit value the MSB (most significant bit) is used to switch to next type. This means in 1b mode, there are 7 bits available for data.
 * Once we expand to 2b value, the MSB must be 1, and for further expansion the bit next to MSB will be used to toggle the 4b mode.
 * To maximize the available values, the duplicated values are removed (when we overflow from one mode to next, it resumes from the last number) at cost of readability (we need a converter to convert back to actual value).
 * After the value is decoded, it will have been stripped of extra bits, so only actual data is represented.
 *  *Due to not being needed ATM, 8b mode has not been implemented!*
 *
 * Example:
 * 0b00000001 (0x01, 1)                     => 0x01, 1 (1b mode)
 * 0b01000000 (0x40, 64)                    => 0x40, 64 (1b mode)
 * 0b01111111 (0x7F, 127)                   => 0x7F, 127 (1b mode)
 * 0b10000000 0b00000000 (0x8000, 32768)    => 0x0080, 128 (2b mode)
 * 0b10000000 0b10000000 (0x8080, 32896)    => 0x0100, 256 (2b mode)
 * 0b10111111 0b11111111 (0xBFFF, 49151)   => 0x407F, 16511 (2b mode)
 * 0b11000000 0b00000000 0b00000000 0b00000000 (0xC0000000, 2147483648) => 0x00004080, 16512 (4b mode)
 * 0b11000000 0b10000000 0b10000000 0b10000000 (0xC0808080, 3229646976) => 0x0080C100, 8438016 (4b mode)
 * 0b11011111 0b11111111 0b11111111 0b11111111 (0xDFFFFFFF, 3758096383) => 0x2000407F, 536887423 (4b mode)
 */

__EXTERN_C_BEGIN

/**
 * Parses the dynamic value.
 * @param input Input dynamic value.
 * @param out_ptr The pointer to where the dynamic size will be written.
 * @returns Parsed value.
 */
uint64_t dynamic_parse(const uint64_t input, uint8_t* dynamic_size_out);

/**
 * Parses the dynamic value from buffer.
 * @param input Input buffer.
 * @param out_ptr The pointer to where the end address of dynamic value will be written.
 * @returns Parsed value.
 */
uint64_t dynamic_parse_buffer(const uint8_t* input, uint8_t** out_ptr);

/**
 * Calculates the required bytes to parse the value.
 * @param first_byte_input First byte of the input (encoded) value.
 * @returns The required bytes.
 */
uint8_t dynamic_parse_get_required_bytes(const uint8_t first_byte_input);

/**
 * Converts the value into dynamic value. The result is returned as single value that can be bit-shifted to obtain each byte.
 * @param value The input value to convert.
 * @param  written_bytes The pointer to where the number of required bytes will be written.
 * @returns Combined converted value.
 */
uint64_t dynamic_serialize(const uint64_t value, uint8_t* written_bytes);

/**
 * Calculates the required bytes to serialize the value into dynamic value.
 * @param value The input value.
 * @returns The required bytes count.
 */
uint8_t dynamic_serialize_get_required_bytes(const uint64_t value);

__EXTERN_C_END

#endif  // UUTILS_DYNAMIC_VALUE_H_
