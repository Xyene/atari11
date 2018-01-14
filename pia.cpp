#include "pia.h"

pia::pia(cpu &cpu) {
    // PIA RAM
    cpu.map_memory_handler(PIA_MASK,
                           PIA_RAM_SELECT,
                           [this](uint16_t addr) -> uint8_t {
                               return ram[addr & 0x7F];
                           },
                           [this](uint16_t addr, uint8_t val) {
                               ram[addr & 0x7F] = val;
                           });

    // PIA ports
    cpu.map_memory_handler(PIA_MASK,
                           PIA_PORT_SELECT,
                           [this](uint16_t addr) -> uint8_t {

                           },
                           [this](uint16_t addr, uint8_t val) {

                           });
}
