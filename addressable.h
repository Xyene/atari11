#ifndef INC_2600_ADDRESSABLE_H
#define INC_2600_ADDRESSABLE_H

#include <functional>
#include <iostream>

typedef std::function<void(uint16_t, uint8_t)> mem_write_handler;

typedef std::function<uint8_t (uint16_t)> mem_read_handler;

template <int ADDRESS_SIZE>
class addressable {
protected:
    mem_write_handler write_handlers[ADDRESS_SIZE];
    mem_read_handler read_handlers[ADDRESS_SIZE];

    void map_write_handler(uint16_t start, uint16_t end, const mem_write_handler &handler) {
        std::cout << sizeof write_handlers << std::endl;
        for (int i = start; i < end; i++) write_handlers[i] = handler;
    }

    void map_read_handler(uint16_t start, uint16_t end, const mem_read_handler &handler) {
        for (int i = start; i < end; i++) read_handlers[i] = handler;
    }
};

#endif //INC_2600_ADDRESSABLE_H
