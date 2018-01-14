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
                               switch (addr & 0xF7) {
                                   case 0x82:
                                       return SWCHB();
                                   case 0x84:
                                       return INTIM();
                                   case 0x85:
                                       return INSTAT();
                                   default:
                                       throw std::logic_error("unhandled PIA read");
                               }
                           },
                           [this](uint16_t addr, uint8_t val) {
                               addr &= 0x7;
                               switch (addr) {
                                   case 0x04:
                                   case 0x05:
                                   case 0x06:
                                   case 0x07:
                                       TIMnnT(addr, val);
                                       return;
                                   default:
                                       throw std::logic_error("unhandled PIA write");
                               }
                           });
}

void pia::step() {
    timer_counter_--;
    printf("Remaining timer cycles: %d\n", timer_counter_);
    if (!timer_counter_) {
        step_timer();
    }
}

void pia::step_timer() {
    printf("Decrementing timer: %d\n", INTIM_ - 1);
    if (!INTIM_) {
        // Bit 6 and 7 set on underflow
        INSTAT_ |= 0xC0;
        // When INTIM underflows, the timer interval is temporarily
        // set to 1
        timer_counter_ = effective_timer_interval_ = 1;
        printf("Underflow!\n");
    } else {
        timer_counter_ = effective_timer_interval_;
    }
    // Underflow when = 0x00 over to 0xFF
    INTIM_--;
}

void pia::TIMnnT(uint16_t addr, uint8_t val) {
    INTIM_ = val;
    const uint16_t timer_lengths[] = {1, 8, 64, 1024};
    timer_counter_ = effective_timer_interval_ = TIMnnT_ = timer_lengths[addr - 0x04];
    printf("Configured timer to %d clock cycles, initial %02X\n", timer_lengths[addr - 0x04], INTIM_);

    // Bit 7 of INSTAT_ is reset when writing to TIM1T ... TIM1024T
    INSTAT_ &= ~0x80;

    // Immediately step the timer (i.e. initial_value = 0x00 underflows
    // to 0xFF instantly)
    step_timer();
}

uint8_t pia::INTIM() {
    // If currently on 1-cycle intervals, it's
    // likely that it's because the timer overflowed and we switched
    // to 1-cycle resolution. Here, reset back to the original timing length.
    if (effective_timer_interval_ == 1) {
        timer_counter_ = TIMnnT_;
        effective_timer_interval_ = TIMnnT_;
    }
    return INTIM_;
}

uint8_t pia::INSTAT() {
    // Bit 6 is reset when reading
    INSTAT_ &= ~0x40;
    return INSTAT_ | 0x40;
}

uint8_t pia::SWCHB() {
    return 0x0B;
}
