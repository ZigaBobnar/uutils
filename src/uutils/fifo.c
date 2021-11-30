#include "uutils/fifo.h"

fifo_t* fifo_create(size_t queue_size) {
    fifo_t* fifo = malloc(sizeof(fifo_t));
    uint8_t* data = calloc(queue_size + 1, sizeof(uint8_t));

    fifo->buffer_start = fifo->read_ptr = fifo->write_ptr = data;
    fifo->buffer_end = data + queue_size;

    return fifo;
}

void fifo_destroy(fifo_t* fifo) {
    free(fifo->buffer_start);
    free(fifo);
}

size_t fifo_read(fifo_t* fifo, uint8_t* data_buffer, size_t n) {
    size_t bytes_read = 0;
    uint8_t* data_ptr = data_buffer;

    while (bytes_read < n && fifo->read_ptr != fifo->write_ptr) {
        *data_ptr = *fifo->read_ptr;

        if (fifo->read_ptr >= fifo->buffer_end) {
            fifo->read_ptr = fifo->buffer_start;
        } else {
            fifo->read_ptr++;
        }

        data_ptr++;
        bytes_read++;
    }

    return bytes_read;
}

bool fifo_read_single(fifo_t* fifo, uint8_t* item_ptr) {
    if (fifo->read_ptr == fifo->write_ptr)
        return false;

    if (item_ptr != NULL)
        *item_ptr = *fifo->read_ptr;

    if (fifo->read_ptr >= fifo->buffer_end) {
        fifo->read_ptr = fifo->buffer_start;
    } else {
        fifo->read_ptr++;
    }

    return true;
}

size_t fifo_peek(fifo_t* fifo, uint8_t* data_buffer, size_t n) {
    size_t bytes_read = 0;
    uint8_t* data_ptr = data_buffer;
    uint8_t* peek_ptr = fifo->read_ptr;

    while (bytes_read < n && peek_ptr != fifo->write_ptr) {
        *data_ptr = *peek_ptr;

        if (peek_ptr >= fifo->buffer_end) {
            peek_ptr = fifo->buffer_start;
        } else {
            peek_ptr++;
        }

        data_ptr++;
        bytes_read++;
    }

    return bytes_read;
}

bool fifo_peek_single(fifo_t* fifo, uint8_t* item_ptr) {
    if (fifo->read_ptr == fifo->write_ptr)
        return false;

    *item_ptr = *fifo->read_ptr;

    return true;
}

size_t fifo_write(fifo_t* fifo, uint8_t* data_buffer_ptr, size_t n) {
    if (fifo_is_full(fifo) || n == 0) {
        return 0;
    }

    size_t bytes_written = 0;
    uint8_t* data_ptr = data_buffer_ptr;

    while (bytes_written < n && !fifo_is_full(fifo)) {
        *fifo->write_ptr = *data_ptr;

        if (fifo->write_ptr >= fifo->buffer_end) {
            fifo->write_ptr = fifo->buffer_start;
        } else {
            fifo->write_ptr++;
        }

        data_ptr++;
        bytes_written++;
    }

    if (fifo->read_ptr == fifo->write_ptr) {
        // Rollback if read and write are equal
        fifo->write_ptr = fifo->write_ptr == fifo->buffer_start ?
            fifo->buffer_end :
            fifo->read_ptr - 1;
    }

    return bytes_written;
}

bool fifo_write_single(fifo_t* fifo, uint8_t value) {
    if (fifo_is_full(fifo)) {
        return false;
    }

    *fifo->write_ptr = value;
    
    if (fifo->write_ptr >= fifo->buffer_end) {
        fifo->write_ptr = fifo->buffer_start;
    } else {
        fifo->write_ptr++;
    }

    return true;
}

void fifo_reset(fifo_t* fifo) {
    fifo->read_ptr = fifo->write_ptr = fifo->buffer_start;
}

bool fifo_has_next_item(fifo_t* fifo) {
    return (fifo->read_ptr != fifo->write_ptr);
}

bool fifo_is_full(fifo_t* fifo) {
    return (fifo->write_ptr == fifo->read_ptr - 1) ||
        (fifo->write_ptr == fifo->buffer_end && fifo->read_ptr == fifo->buffer_start);
}

size_t fifo_size(fifo_t* fifo) {
    return fifo->buffer_end - fifo->buffer_start;
}

bool fifo_valid(fifo_t* fifo) {
    return fifo != NULL &&
        fifo->read_ptr <= fifo->buffer_end &&
        fifo->write_ptr <= fifo->buffer_end &&
        fifo->read_ptr >= fifo->buffer_start &&
        fifo->write_ptr >= fifo->buffer_start;
}
