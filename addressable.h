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

public:
    addressable() {
        std::fill(write_handlers, write_handlers + ADDRESS_SIZE, nullptr);
        std::fill(read_handlers, read_handlers + ADDRESS_SIZE, nullptr);
    }

    void map_write_handler(uint16_t start, uint16_t end, const mem_write_handler &handler) {
        std::fill(write_handlers + start, write_handlers + end, handler);
    }

    void map_read_handler(uint16_t start, uint16_t end, const mem_read_handler &handler) {
        std::fill(read_handlers + start, read_handlers + end, handler);
    }

    uint8_t read8(uint16_t addr) const {
        addr &= ADDRESS_SIZE;
        return read_handlers[addr](addr);
    }

    uint16_t read16(uint16_t addr) const {
        return read8(addr) | (read8(addr + 1) << 8);
    }

    void write8(uint16_t addr, uint8_t val) const {
        addr &= ADDRESS_SIZE;
        write_handlers[addr](addr, val);
    }

    void write16(uint16_t addr, uint16_t val) const {
        write8(addr, val);
        write8(addr + 1, val >> 8);
    }
};

#endif //INC_2600_ADDRESSABLE_H
