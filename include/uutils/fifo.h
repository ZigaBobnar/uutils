#ifndef UUTILS_FIFO_H_
#define UUTILS_FIFO_H_

#include "common.h"

/**
 * FIFO (First In, First Out) buffer
 *
 * Fifo cyclic buffer can be used whenewer a program needs to always append
 * data after the current position in memory without the need for infinite
 * memory size. The buffer can be read all the way to the most recently written
 * data. Existing buffer data must be read (read index advances) before it can
 * be overwritten.
 */

__EXTERN_C_BEGIN

/**
 * Fifo structure
 */
typedef struct _fifo {
    /** Current read pointer */
    uint8_t* read_ptr;
    /** Current write pointer */
    uint8_t* write_ptr;

    /** Pointer to the start of data buffer */
    uint8_t* buffer_start;
    /** Pointer to the end of data buffer */
    uint8_t* buffer_end;
} fifo_t;

/**
 * Initialize a new fifo structure with set buffer and return pointer to it.
 * When done using the structure must be properly disposed by calling fifo_destroy.
 * @param queue_size The size of fifo queue
 * @returns Pointer to created fifo structure
 */
fifo_t* fifo_create(size_t queue_size);

/**
 * Destroy a fifo structure.
 * @param fifo Fifo structure
 */
void fifo_destroy(fifo_t* fifo);

/**
 * Read from fifo buffer
 * @param fifo Fifo buffer to read data from
 * @param data_buffer Pointer to where fifo data should be read
 * @param n Number of bytes to read from buffer
 * @returns Number of bytes that were read
 */
size_t fifo_read(fifo_t* fifo, uint8_t* data_buffer, size_t n);

/**
 * Read single item from fifo buffer
 * @param fifo Fifo buffer
 * @param item_ptr Pointer to where the fifo data should be read
 * @returns Whether the item was read
 */
bool fifo_read_single(fifo_t* fifo, uint8_t* item_ptr);

/**
 * Peeks (reads without advancing read pointer) the fifo buffer
 * @param fifo Fifo buffer to read data from
 * @param data_buffer Pointer to where fifo data should be read
 * @param n Number of bytes to read from buffer
 * @returns Number of bytes that were read
 */
size_t fifo_peek(fifo_t* fifo, uint8_t* data_buffer, size_t n);

/**
 * Peeks single item from fifo buffer
 * @returns Whether the item was read
 */
bool fifo_peek_single(fifo_t* fifo, uint8_t* item_ptr);

/**
 * Write data into fifo buffer
 * @param fifo Fifo buffer to write data to
 * @param data_buffer_ptr Pointer to data that needs to be written into fifo buffer
 * @param n Number of bytes to write to buffer
 * @returns Number of bytes that were written
 */
size_t fifo_write(fifo_t* fifo, uint8_t* data_buffer_ptr, size_t n);

/**
 * Write single byte into buffer
 * @param fifo Fifo buffer to write data to
 * @param value Value that needs to be written into fifo buffer
 * @returns Whether the value was written
 */
bool fifo_write_single(fifo_t* fifo, uint8_t value);

/**
 * Discard any unread data
 * This will set the read and write pointers to the start of buffer in order to
 * reset fifo
 * @param fifo Fifo buffer to discard
 */
void fifo_reset(fifo_t* fifo);

/**
 * Checks whether one more item is available to be read from fifo buffer
 * @param fifo Fifo buffer to check
 * @returns True if at least one item can be read from fifo
 */
bool fifo_has_next_item(fifo_t* fifo);

/**
 * Checks whether any data can be written to fifo
 * @param fifo Fifo buffer
 * @returns True if at least single item can be written to fifo
 */
bool fifo_is_full(fifo_t* fifo);

/**
 * Get the size of fifo
 * @param fifo Fifo buffer
 * @returns The size of fifo buffer
 */
size_t fifo_size(fifo_t* fifo);

/**
 * Check the integrity of fifo buffer by validating that the pointers are in valid range
 * @param fifo Fifo buffer
 * @returns True if fifo is valid
 */
bool fifo_valid(fifo_t* fifo);

__EXTERN_C_END

#endif  // UUTILS_FIFO_H_
