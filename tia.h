#ifndef INC_2600_TIA_H
#define INC_2600_TIA_H

#include "cpu.h"

const uint16_t TIA_MASK = A12 | A7;
const uint16_t TIA_SELECT = 0;

class tia {
public:
    explicit tia(cpu &cpu);
};


#endif //INC_2600_TIA_H
