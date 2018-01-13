#include "tia.h"

tia::tia(cpu &cpu) {
    cpu.map_write_handler(0x0000, 0x002C, [this](uint16_t addr, uint8_t val) {

    });
    cpu.map_read_handler(0x0000, 0x000D, [this](uint16_t addr) -> uint8_t {

    });
}
