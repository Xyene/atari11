#ifndef INC_2600_PIA_H
#define INC_2600_PIA_H

#include "cpu.h"

const uint16_t PIA_MASK = A12 | A9 | A7;
const uint16_t PIA_RAM_SELECT = A7;
const uint16_t PIA_PORT_SELECT = A9 | A7;

class pia {
    uint8_t ram[128];

    // Timer controls
    uint16_t timer_counter_ = 1024;
    uint16_t effective_timer_interval_ = 1024;
    uint16_t TIMnnT_ = 1024;

    uint8_t INTIM_ = 0;
    uint8_t INSTAT_ = 0;

public:
    explicit pia(cpu &cpu);

    void step();

protected:
    void step_timer();

    inline void TIMnnT(uint16_t addr, uint8_t val);

    inline uint8_t INTIM();

    inline uint8_t INSTAT();

    inline uint8_t SWCHB();
};


#endif //INC_2600_PIA_H
