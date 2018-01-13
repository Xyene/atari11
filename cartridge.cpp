#include "cartridge.h"

cartridge::cartridge(cpu cpu, const char *path) {
    FILE *rom_file = fopen(path, "rb");
    // TODO: proper length
    fread(rom, sizeof(uint8_t), 0x1000, rom_file);

    cpu.map_read_handler(0x1000, 0x1FFF, [this](uint16_t addr) -> uint8_t {
        printf("Reading ROM at %04X\n", addr);
        return rom[addr - 0x1000];
    });
}
