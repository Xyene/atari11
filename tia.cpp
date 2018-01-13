#include "tia.h"

tia::tia(cpu &cpu) {
    for (int mirror = 0x0000; mirror < 0x1000; mirror += 0x100)
        for (int step = 0; step < 2; step++)
            cpu.map_write_handler(0x0000 + mirror + step * 0x40, 0x002C + mirror + step * 0x40,
                                  [this](uint16_t addr, uint8_t val) {
                                      addr &= 0x3F;
                                  });
    for (int mirror = 0x0000; mirror < 0x1000; mirror += 0x100)
        for (int step = 0; step < 8; step++)
            cpu.map_read_handler(0x0000 + mirror + step * 0x10, 0x000D + mirror + step * 0x10,
                                 [this](uint16_t addr) -> uint8_t {
                                    addr &= 0xF;
                                 });
}
