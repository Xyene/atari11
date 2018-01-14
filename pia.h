#ifndef INC_2600_PIA_H
#define INC_2600_PIA_H

#include "cpu.h"

const uint16_t PIA_MASK = A12 | A9 | A7;
const uint16_t PIA_RAM_SELECT = A7;
const uint16_t PIA_PORT_SELECT = A9 | A7;

class pia {
    uint8_t ram[128];
public:
    explicit pia(cpu &cpu);
};


#endif //INC_2600_PIA_H
