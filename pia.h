#ifndef INC_2600_PIA_H
#define INC_2600_PIA_H

#include "cpu.h"

class pia {
    uint8_t ram[128];
public:
    explicit pia(cpu &cpu);
};


#endif //INC_2600_PIA_H
