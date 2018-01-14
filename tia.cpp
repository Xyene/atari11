#include "tia.h"

tia::tia(cpu &cpu) {
    cpu.map_memory_handler(TIA_MASK,
                           TIA_SELECT,
                           [this](uint16_t addr) -> uint8_t {
                               // A0 - A3
                               addr &= 0xF;
                           },
                           [this](uint16_t addr, uint8_t val) {
                               // A0 - A5
                               addr &= 0x3F;
                           });
}
