#ifndef UUTILS_FIFO_HPP_
#define UUTILS_FIFO_HPP_

#include "fifo.h"

#include <vector>

class fifo {
public:
    inline fifo(size_t queue_size) {
        _fifo = fifo_create(queue_size);
    }

    inline ~fifo() {
        fifo_destroy(_fifo);
    }

    inline fifo_t* get() {
        return _fifo;
    }

    inline std::vector<uint8_t> read(size_t n) {
        std::vector<uint8_t> result;

        result.reserve(n);
        size_t count = fifo_read(_fifo, result.data(), n);
        result.resize(count);

        return result;
    }

    inline uint8_t read_single() {
        uint8_t value = 0;

        bool data_read = fifo_read_single(_fifo, &value);

        return value;
    }

    inline std::vector<uint8_t> peek(size_t n) {
        std::vector<uint8_t> result;

        result.reserve(n);
        size_t count = fifo_peek(_fifo, result.data(), n);
        result.resize(count);

        return result;
    }

    inline uint8_t peek_single() {
        uint8_t value = 0;

        bool data_read = fifo_peek_single(_fifo, &value);

        return value;
    }

    inline size_t write(std::vector<uint8_t> data) {
        return fifo_write(_fifo, data.data(), data.size());

    }

    inline bool write_single(uint8_t value) {
        return fifo_write_single(_fifo, value);
    }

    inline bool reset() {
        fifo_reset(_fifo);
    }

    inline bool has_next_item() {
        return fifo_has_next_item(_fifo);
    }

    inline bool is_full() {
        return fifo_is_full(_fifo);
    }

    inline size_t size() {
        return fifo_size(_fifo);
    }

    inline bool valid() {
        return fifo_valid(_fifo);
    }

private:
    fifo_t* _fifo;
};

#endif  // UUTILS_FIFO_HPP_
