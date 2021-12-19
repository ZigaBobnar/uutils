#ifndef UUTILS_DYNAMIC_VALUE_H_
#define UUTILS_DYNAMIC_VALUE_H_

#include "common.h"

/**
 * Extensible numbers system
 *
 * Those types are used when there is a possibility to start with smaller sized data type, but it might require to grow in length.
 * The notation for data size is as such: [1b,2b,4b,8b], meaning data can grow from simple 8-bit (1 byte) value all the way to 64-bits (8 bytes).
 * In reality the value notation is a bit misleading as in 8-bit value the 2 MSB (most significant bits) are used to for signedness and to switch to next size type. This means in 1b mode, there are 6 bits available for data.
 * Once we expand to 2b value, the second MSB must be 1, and for further expansion the bit next to that MSB will be used to toggle the 4b mode.
 * To maximize the available values, the duplicated values are removed (when we overflow from one mode to next, it resumes from the last number) at cost of readability (we need a converter to convert back to actual value).
 * After the value is decoded, it will have been stripped of extra bits, so only actual data is represented.
 * Negative values follow two's complement - active bits are 0 and inactive 1
 *
 * Example:
 * 
 * - 1b mode:
 * 0bsnxxxxxx
 *      s => sign
 *      n => 1b2b switch
 *
 * 0d00 = 0b00000000 =>
 *      0x00 = 0b00000000 = 0
 * 0d15 =>
 *      0x15
 * 0d3F = 0b00111111 =>
 *      0x3F = 0x00111111 = 63 = 1b_max
 *
 * 0dFF = 0b11111111 =>
 *      0xFF = 0b11111111 = -1
 * 0dC0 = 0b11000000 =>
 *      0xC0 = 0b11000000 = -64 = 1b_min
 *
 * 
 * - 2b mode:
 * 0bsanxxxxx 0bxxxxxxxx
 *      s => sign
 *      a => always active
 *      n => 2b4b switch
 *
 * 0d4000 = 0b01000000 0b00… =>
 *      0x0040 = 64 = 2b_pos_offset
 * 0d5555 =>
 *      0x1595
 * 0d5FFF = 0b01011111 0b11… =>
 *      0x203F = 8255 = 2b_max
 *
 * 0dBFFF = 0b10111111 0b11… =>
 *      0xFFBF = -65 = 2b_neg_offset
 * 0dAAAA =>
 *      0xEA6A
 * 0dA000 = 0b10100000 0b00… =>
 *      0xDFC0 = -8256 = 2b_min
 *
 * 
 * - 4b mode:
 * 0bsaanxxxx 0bxxxxxxxx 0bxxxxxxxx 0bxxxxxxxx
 *      s => sign
 *      a => always active
 *      n => 4b8b switch
 *
 * 0d60000000 = 0b01100000 0b00… =>
 *      0x00002040 = 8256 = 4b_pos_offset
 * 0d6AAAAAAA =>
 *      0x0AAACAEA
 * 0d6FFFFFFF = 0b01101111 0b11… =>
 *      0x1000203F = 268443711 = 4b_max
 *
 * 0d9FFFFFFF = 0b10011111 0b11… =>
 *      0xFFFFDFBF = -8257 = 4b_neg_offset
 * 0d95555555 =>
 *      0xF5553515
 * 0d90000000 = 0b10010000 0b00… =>
 *      0xEFFFDFC0 = -268443712 = 4_bmin
 * 
 * 
 * - 8b mode:
 * 0bsaaanxxx 0bxxxxxxxx 0bxxxxxxxx 0bxxxxxxxx 0bxxxxxxxx 0bxxxxxxxx 0bxxxxxxxx 0bxxxxxxxx
 *      s => sign
 *      a => always active
 *      n => 8b16b switch
 *
 * 0d7000000000000000 = 0b01110000 0b00… =>
 *      0x0000000010002040 = 268443712 = 8bpos offset
 * 0d7555555555555555 =>
 *      0x0555555565557595
 * 0d77FFFFFFFFFFFFFF = 0b01101111 0b11… =>
 *      0x080000001000203F = 576460752571867199 = 8bmax
 * 
 * 0d8FFFFFFFFFFFFFFF = 0b10001111 0b11… =>
 *      0xFFFFFFFFEFFFDFBF = -268443713 = 8bneg offset
 * 0d8AAAAAAAAAAAAAAA =>
 *      0xFAAAAAAA9AAA8A6A
 * 0d8800000000000000 = 0b10001000 0b00… =>
 *      0xF7FFFFFFEFFFDFC0 = -576460752571867201 = 8bmin
 */

__EXTERN_C_BEGIN

#define DYNAMIC_DEFAULT_MAX_SIZE 4

#ifndef DYNAMIC_MAX_SIZE
#define DYNAMIC_MAX_SIZE DYNAMIC_DEFAULT_MAX_SIZE
#endif

#if DYNAMIC_MAX_SIZE == 16
#error 16 byte dynamic size is not available on this platform.
#elif DYNAMIC_MAX_SIZE == 8
typedef int64_t dynamic_real;
typedef uint64_t dynamic;
#elif DYNAMIC_MAX_SIZE == 4
typedef int32_t dynamic_real;
typedef uint32_t dynamic;
#elif DYNAMIC_MAX_SIZE == 2
typedef int16_t dynamic_real;
typedef uint16_t dynamic;
#elif (DYNAMIC_MAX_SIZE == 1) || (DYNAMIC_MAX_SIZE == 0)
#error Dynamic max size is too small.
#else
#error Dynamic max size is invalid!
#endif

#define DYNAMIC_HAS_1B 1
#if DYNAMIC_MAX_SIZE >= 2
#define DYNAMIC_HAS_2B 1
#else
#define DYNAMIC_HAS_2B 0
#endif
#if DYNAMIC_MAX_SIZE >= 4
#define DYNAMIC_HAS_4B 1
#else
#define DYNAMIC_HAS_4B 0
#endif
#if DYNAMIC_MAX_SIZE >= 8
#define DYNAMIC_HAS_8B 1
#else
#define DYNAMIC_HAS_8B 0
#endif
#if DYNAMIC_MAX_SIZE >= 16
#define DYNAMIC_HAS_16B 1
#else
#define DYNAMIC_HAS_16B 0
#endif

/**
 * Parses the dynamic value into real value.
 * @param input Input dynamic value.
 * @param out_ptr The pointer to where the dynamic size will be written.
 * @returns Parsed value.
 */
dynamic_real dynamic_to_real(const dynamic dynamic_input, uint8_t* dynamic_size_out);

/**
 * Parses the dynamic value from buffer.
 * @param input Input buffer.
 * @param out_ptr The pointer to where the end address of dynamic value will be written.
 * @returns Parsed value.
 */
dynamic_real dynamic_buffer_to_real(uint8_t* input, uint8_t** out_ptr);

/**
 * Calculates the required bytes to parse the value.
 * @param first_byte_input First byte of the input (encoded) value.
 * @returns The required bytes.
 */
uint8_t dynamic_to_real_get_required_bytes(const uint8_t first_byte_input);

/**
 * Finds the preamble byte value in the dynamic value.
 * @param dynamic_value The combined dynamic value.
 * @returns The preamble.
 */
uint8_t dynamic_find_preamble_byte(const dynamic dynamic_value);

/**
 * Converts the value into dynamic value. The result is returned as single value that can be bit-shifted to obtain each byte.
 * @param value The input value to convert.
 * @param  written_bytes The pointer to where the number of required bytes will be written.
 * @returns Combined converted value.
 */
dynamic real_to_dynamic(const dynamic_real value, uint8_t* written_bytes);

/**
 * Calculates the required bytes to serialize the value into dynamic value.
 * @param value The input value.
 * @returns The required bytes count.
 */
uint8_t real_to_dynamic_get_required_bytes(const dynamic_real value);

uint8_t real_to_dynamic_buffer(dynamic_real value, uint8_t* buffer);

typedef struct _dynamic_state {
    uint8_t required_num;
    uint8_t cached_num;
    uint8_t cache[DYNAMIC_MAX_SIZE];
} dynamic_state_t;

bool dynamic_to_real_stateful(dynamic_state_t* state, uint8_t value, dynamic_real* return_ptr);

__EXTERN_C_END

#endif  // UUTILS_DYNAMIC_VALUE_H_
