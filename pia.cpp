#include "pia.h"

pia::pia(cpu &cpu) {
    // RAM
    cpu.map_write_handler(0x0080, 0x00FF, [this](uint16_t addr, uint8_t val) {
        ram[addr - 0x80] = val;
    });
    cpu.map_read_handler(0x0080, 0x00FF, [this](uint16_t addr) -> uint8_t {
        return ram[addr - 0x80];
    });

    // PIA ports
    cpu.map_write_handler(0x0280, 0x0297, [this](uint16_t addr, uint8_t val) {

    });
    cpu.map_read_handler(0x0280, 0x0297, [this](uint16_t addr) -> uint8_t {

    });
}
