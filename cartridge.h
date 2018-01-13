#ifndef INC_2600_CARTRIDGE_H
#define INC_2600_CARTRIDGE_H


#include "cpu.h"

class cartridge {
    uint8_t rom[0x1000];
public:
    cartridge(cpu &cpu, const char* path);
    ~cartridge();
};


#endif //INC_2600_CARTRIDGE_H
