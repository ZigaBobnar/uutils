#include "test_core.hpp"

void test_place_dynamic(dynamic value, uint8_t size, uint8_t* buffer) {
    if (size == 8) {
        buffer[0] = (uint8_t)(value >> 56);
        buffer[1] = (uint8_t)(value >> 48);
        buffer[2] = (uint8_t)(value >> 40);
        buffer[3] = (uint8_t)(value >> 32);
        buffer[4] = (uint8_t)(value >> 24);
        buffer[5] = (uint8_t)(value >> 16);
        buffer[6] = (uint8_t)(value >> 8);
        buffer[7] = (uint8_t)(value);
    } else if (size == 4) {
        buffer[0] = (uint8_t)(value >> 24);
        buffer[1] = (uint8_t)(value >> 16);
        buffer[2] = (uint8_t)(value >> 8);
        buffer[3] = (uint8_t)(value);
    } else if (size == 2) {
        buffer[0] = (uint8_t)(value >> 8);
        buffer[1] = (uint8_t)(value);
    } else if (size == 1) {
        buffer[0] = (uint8_t)(value);
    } else {
        EXPECT_TRUE(false);
    }
}
