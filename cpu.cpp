#pragma GCC diagnostic ignored "-Wconversion"

#include "cpu.h"

opcode_handler cpu::opcode_handlers[256];

std::string cpu::opcode_names[256];

opcode_def cpu::opcode_defs[256];

void cpu::step() {
    (this->*opcode_handlers[0x20])();
}

void cpu::reset() {

}

uint8_t cpu::operand() const {
    return 0;
}

void cpu::operand(uint8_t val) {

}

uint8_t cpu::read8(uint16_t addr) const {
    return 0;
}

uint16_t cpu::read16(uint16_t addr) const {
    return 0;
}

void cpu::write8(uint16_t addr, uint8_t val) {

}

void cpu::write16(uint16_t addr, uint16_t val) {

}

void cpu::set_flags(uint8_t val) {

}

void cpu::set_flag_if(uint8_t mask, bool cond) {

}

void cpu::push16(uint16_t val) {

}

void cpu::push8(uint8_t val) {

}

uint8_t cpu::pop8() { return 0; }

uint16_t cpu::pop16() { return 0; }

uint16_t cpu::next16() { return 0; }

uint8_t cpu::next8() { return 0; }

int8_t cpu::nexts8() { return 0; }

#define OPCODE(x, ...) _register_opcode opcode_##x##_def(#x, &cpu::op_##x, {__VA_ARGS__}); \
                       void cpu::op_##x()

#define FLAG_NEGATIVE       (P & NEGATIVE_BIT)
#define FLAG_OVERFLOW       (P & OVERFLOW_BIT)
#define FLAG_BRK_SOURCE     (P & BRK_SOURCE_BIT)
#define FLAG_DECIMAL_MODE   (P & DECIMAL_MODE_BIT)
#define FLAG_INT_DISABLED   (P & INT_DISABLED_BIT)
#define FLAG_ZERO           (P & ZERO_BIT)
#define FLAG_CARRY          (P & CARRY_BIT)

OPCODE(JSR,
       { .opcode = 0x20, .cycles = 6 }
) {
    push16(PC + 1);
    PC = next16();
}

OPCODE(RTI,
       { .opcode = 0x40, .cycles = 6 }
) {
    next8();
    P = pop8();
    PC = pop16();
}

OPCODE(RTS,
       { .opcode = 0x60, .cycles = 6 }
) {
    next8();
    PC = pop16() + 1;
}

OPCODE(INY,
       { .opcode = 0xC8, .cycles = 2 }
) {
    set_flags(Y++);
}

OPCODE(DEY,
       { .opcode = 0x88, .cycles = 2 }
) {
    set_flags(Y--);
}

OPCODE(INX,
       { .opcode = 0xE8, .cycles = 2 }
) {
    set_flags(X++);
}

OPCODE(DEX,
       (opcode_def) {.opcode = 0xCA, .cycles = 2, .rmw = true}
) {
    set_flags(X--);
}

OPCODE(TAY,
       { .opcode = 0xA8, .cycles = 2 }
) {
    set_flags(Y = A);
}

OPCODE(TYA,
       { .opcode = 0x98, .cycles = 2 }
) {
    set_flags(A = Y);
}

OPCODE(TAX,
       { .opcode = 0xA8, .cycles = 2, .rmw = true }
) {
    set_flags(A = X);
}

OPCODE(TXA,
       { .opcode = 0x8A, .cycles = 2, .rmw = true }
) {
    set_flags(X = Y);
}

OPCODE(TSX,
       { .opcode = 0xBA, .cycles = 2 }
) {
    set_flags(X = SP);
}

OPCODE(TXS,
       { .opcode = 0x9A, .cycles = 2, .rmw = true }
) {
    set_flags(SP = X);
}

OPCODE(PHP,
       { .opcode = 0x08, .cycles = 3 }
) {
    push8(P | BRK_SOURCE_BIT);
}

OPCODE(PLP,
       { .opcode = 0x28, .cycles = 4 }
) {
    P = pop8() & ~BRK_SOURCE_BIT;
}

OPCODE(PLA,
       { .opcode = 0x68, .cycles = 4 }
) {
    set_flags(A = pop8());
}

OPCODE(PHA,
       { .opcode = 0x48, .cycles = 3 }
) {
    push8(A);
}

OPCODE(BIT,
       { .opcode = 0x24, .mode = Absolute, .cycles = 3 },
       { .opcode = 0x2C, .mode = ZeroPage, .cycles = 4 }
) {
    uint8_t val = operand();
    set_flag_if(OVERFLOW_BIT, val & 0x40);
    set_flag_if(ZERO_BIT, !(val & A));
    set_flag_if(NEGATIVE_BIT, val & 0x80);
}

void cpu::branch_if(bool cond) {
    auto new_pc = (uint8_t) (PC + nexts8() + 1);
    if (cond) {
        PC = new_pc;
        cycle++;
    }
}

OPCODE(BCS,
       { .opcode = 0xB0, .cycles = 2 }
) {
    branch_if(FLAG_CARRY);
}

OPCODE(BCC,
       { .opcode = 0x90, .cycles = 2 }
) {
    branch_if(!FLAG_CARRY);
}

OPCODE(BEQ,
       { .opcode = 0xF0, .cycles = 2 }
) {
    branch_if(FLAG_ZERO);
}

OPCODE(BNE,
       { .opcode = 0xD0, .cycles = 2 }
) {
    branch_if(!FLAG_ZERO);
}

OPCODE(BVS,
       { .opcode = 0x70, .cycles = 2 }
) {
    branch_if(FLAG_OVERFLOW);
}

OPCODE(BVC,
       { .opcode = 0x50, .cycles = 2 }
) {
    branch_if(!FLAG_OVERFLOW);
}

OPCODE(BPL,
       { .opcode = 0x10, .cycles = 2 }
) {
    branch_if(!FLAG_NEGATIVE);
}

OPCODE(BMI,
       { .opcode = 0x30, .cycles = 2 }
) {
    branch_if(FLAG_NEGATIVE);
}

OPCODE(STA,
       { .opcode = 0x81, .mode = IndirectX, .cycles = 6 },
       { .opcode = 0x91, .mode = IndirectY, .cycles = 6 },
       { .opcode = 0x95, .mode = ZeroPageX, .cycles = 4 },
       { .opcode = 0x99, .mode = AbsoluteY, .cycles = 5 },
       { .opcode = 0x9D, .mode = AbsoluteX, .cycles = 5 },
       { .opcode = 0x85, .mode = ZeroPage, .cycles = 3 },
       { .opcode = 0x8D, .mode = Absolute, .cycles = 4 }
) {
    operand(A);
}

OPCODE(STX,
       { .opcode = 0x96, .mode = ZeroPageY, .cycles = 4 },
       { .opcode = 0x86, .mode = ZeroPage, .cycles = 3 },
       { .opcode = 0x8E, .mode = Absolute, .cycles = 4 }
) {
    operand(X);
}

OPCODE(STY,
       { .opcode = 0x94, .mode = ZeroPageX, .cycles = 4 },
       { .opcode = 0x84, .mode = ZeroPage, .cycles = 3 },
       { .opcode = 0x8C, .mode = Absolute, .cycles = 4 }
) {
    operand(Y);
}

OPCODE(CLC,
       { .opcode = 0x18, .cycles = 2 }
) {
    set_flag_if(CARRY_BIT, false);
}

OPCODE(SEC,
       { .opcode = 0x38, .cycles = 2 }
) {
    set_flag_if(CARRY_BIT, true);
}

OPCODE(CLI,
       { .opcode = 0x58, .cycles = 2 }
) {
    set_flag_if(INT_DISABLED_BIT, false);
}

OPCODE(SEI,
       { .opcode = 0x78, .cycles = 2 }
) {
    set_flag_if(INT_DISABLED_BIT, true);
}

OPCODE(CLV,
       { .opcode = 0xB8, .cycles = 2 }
) {
    set_flag_if(OVERFLOW_BIT, false);
}

OPCODE(CLD,
       { .opcode = 0xD8, .cycles = 2 }
) {
    set_flag_if(DECIMAL_MODE_BIT, false);
}

OPCODE(SED,
       { .opcode = 0xF8, .cycles = 2 }
) {
    set_flag_if(DECIMAL_MODE_BIT, true);
}

OPCODE(NOP,
       { .opcode = 0xEA, .cycles = 2 }
) {
    // Do nothing
}

OPCODE(LDA,
       { .opcode = 0xA1, .mode = IndirectX, .cycles = 6 },
       { .opcode = 0xA5, .mode = ZeroPage, .cycles = 3 },
       { .opcode = 0xA9, .mode = Immediate, .cycles = 2 },
       { .opcode = 0xAD, .mode = Absolute, .cycles = 4 },
       { .opcode = 0xB1, .mode = IndirectY, .cycles = 5, .page_boundary = true },
       { .opcode = 0xB5, .mode = ZeroPageX, .cycles = 4 },
       { .opcode = 0xB9, .mode = AbsoluteY, .cycles = 4, .page_boundary = true },
       { .opcode = 0xBD, .mode = AbsoluteX, .cycles = 4, .page_boundary = true }
) {
    set_flags(A = operand());
}

OPCODE(LDY,
       { .opcode = 0xA0, .mode = Immediate, .cycles = 2 },
       { .opcode = 0xA4, .mode = ZeroPage, .cycles = 3 },
       { .opcode = 0xAC, .mode = Absolute, .cycles = 4 },
       { .opcode = 0xB4, .mode = ZeroPageX, .cycles = 4 },
       { .opcode = 0xBC, .mode = AbsoluteX, .cycles = 4, .page_boundary = true }
) {
    set_flags(Y = operand());
}

OPCODE(LDX,
       { .opcode = 0xA2, .mode = Immediate, .cycles = 2, .rmw = true },
       { .opcode = 0xA6, .mode = ZeroPage, .cycles = 3, .rmw = true },
       { .opcode = 0xAE, .mode = Absolute, .cycles = 4, .rmw = true },
       { .opcode = 0xB6, .mode = ZeroPageY, .cycles = 4, .rmw = true },
       { .opcode = 0xBE, .mode = AbsoluteY, .cycles = 4, .page_boundary = true, .rmw = true }
) {
    set_flags(X = operand());
}

OPCODE(ORA,
       { .opcode = 0x01, .mode = IndirectX, .cycles = 6 },
       { .opcode = 0x05, .mode = ZeroPage, .cycles = 3 },
       { .opcode = 0x09, .mode = Immediate, .cycles = 2 },
       { .opcode = 0x0D, .mode = Absolute, .cycles = 4 },
       { .opcode = 0x11, .mode = IndirectY, .cycles = 5, .page_boundary = true },
       { .opcode = 0x15, .mode = ZeroPageX, .cycles = 4 },
       { .opcode = 0x19, .mode = AbsoluteY, .cycles = 4, .page_boundary = true },
       { .opcode = 0x1D, .mode = AbsoluteX, .cycles = 4, .page_boundary = true }
) {
    set_flags(A |= operand());
}

OPCODE(AND,
       { .opcode = 0x21, .mode = IndirectX, .cycles = 6 },
       { .opcode = 0x25, .mode = ZeroPage, .cycles = 3 },
       { .opcode = 0x29, .mode = Immediate, .cycles = 2 },
       { .opcode = 0x2D, .mode = Absolute, .cycles = 4 },
       { .opcode = 0x31, .mode = IndirectY, .cycles = 5, .page_boundary = true },
       { .opcode = 0x35, .mode = ZeroPageX, .cycles = 4 },
       { .opcode = 0x39, .mode = AbsoluteY, .cycles = 4, .page_boundary = true },
       { .opcode = 0x3D, .mode = AbsoluteX, .cycles = 4, .page_boundary = true }
) {
    set_flags(A &= operand());
}

OPCODE(EOR,
       { .opcode = 0x41, .mode = IndirectX, .cycles = 6 },
       { .opcode = 0x45, .mode = ZeroPage, .cycles = 3 },
       { .opcode = 0x49, .mode = Immediate, .cycles = 2 },
       { .opcode = 0x4D, .mode = Absolute, .cycles = 4 },
       { .opcode = 0x51, .mode = IndirectY, .cycles = 5, .page_boundary = true },
       { .opcode = 0x55, .mode = ZeroPageX, .cycles = 4 },
       { .opcode = 0x59, .mode = AbsoluteY, .cycles = 4, .page_boundary = true },
       { .opcode = 0x5D, .mode = AbsoluteX, .cycles = 4, .page_boundary = true }
) {
    set_flags(A ^= operand());
}

OPCODE(SBC,
       { .opcode = 0xE1, .mode = IndirectX, .cycles = 6 },
       { .opcode = 0xE5, .mode = ZeroPage, .cycles = 3 },
       { .opcode = 0x69, .mode = Immediate, .cycles = 2 },
       { .opcode = 0xE9, .mode = Immediate, .cycles = 2 }, // Official duplicate of $69
       { .opcode = 0xEB, .mode = Immediate, .cycles = 2 }, // Unofficial duplicate of $69
       { .opcode = 0xED, .mode = Absolute, .cycles = 4 },
       { .opcode = 0xF1, .mode = IndirectY, .cycles = 5, .page_boundary = true },
       { .opcode = 0xF5, .mode = ZeroPageX, .cycles = 4 },
       { .opcode = 0xF9, .mode = AbsoluteY, .cycles = 4, .page_boundary = true },
       { .opcode = 0xFD, .mode = AbsoluteX, .cycles = 4, .page_boundary = true }
) { /* TODO */ }

OPCODE(ADC,
       { .opcode = 0x61, .mode = IndirectX, .cycles = 6 },
       { .opcode = 0x65, .mode = ZeroPage, .cycles = 3 },
       { .opcode = 0x69, .mode = Immediate, .cycles = 2 },
       { .opcode = 0x6D, .mode = Absolute, .cycles = 4 },
       { .opcode = 0x71, .mode = IndirectY, .cycles = 5, .page_boundary = true },
       { .opcode = 0x75, .mode = ZeroPageX, .cycles = 4 },
       { .opcode = 0x79, .mode = AbsoluteY, .cycles = 4, .page_boundary = true },
       { .opcode = 0x7D, .mode = AbsoluteX, .cycles = 4, .page_boundary = true }
) { /* TODO */ }

OPCODE(BRK,
       { .opcode = 0x00, .cycles = 7 }
) {
    next8();
    push8(P | BRK_SOURCE_BIT);
    set_flag_if(INT_DISABLED_BIT, true);
    PC = read16(0xFFFE);
}

void cpu::compare(uint8_t val) {

}

OPCODE(CMP,
       { .opcode = 0xC1, .mode = IndirectX, .cycles = 6 },
       { .opcode = 0xC5, .mode = ZeroPage, .cycles = 3 },
       { .opcode = 0xC9, .mode = Immediate, .cycles = 2 },
       { .opcode = 0xCD, .mode = Absolute, .cycles = 4 },
       { .opcode = 0xD1, .mode = IndirectY, .cycles = 5, .page_boundary = true },
       { .opcode = 0xD5, .mode = ZeroPageX, .cycles = 4 },
       { .opcode = 0xD9, .mode = AbsoluteY, .cycles = 4, .page_boundary = true },
       { .opcode = 0xDD, .mode = AbsoluteX, .cycles = 4, .page_boundary = true }
) {
    compare(A);
}

OPCODE(CPX,
       { .opcode = 0xE0, .mode = Immediate, .cycles = 2 },
       { .opcode = 0xE4, .mode = ZeroPage, .cycles = 3 },
       { .opcode = 0xEC, .mode = Absolute, .cycles = 4 }
) {
    compare(X);
}

OPCODE(CPY,
       { .opcode = 0xC0, .mode = Immediate, .cycles = 2 },
       { .opcode = 0xC4, .mode = ZeroPage, .cycles = 3 },
       { .opcode = 0xCC, .mode = Absolute, .cycles = 4 }
) {
    compare(Y);
}

OPCODE(LSR,
       { .opcode = 0x46, .mode = ZeroPage, .cycles = 5, .rmw = true },
       { .opcode = 0x4E, .mode = Absolute, .cycles = 6, .rmw = true },
       { .opcode = 0x56, .mode = ZeroPageX, .cycles = 6, .rmw = true },
       { .opcode = 0x5E, .mode = AbsoluteX, .cycles = 7, .rmw = true },
       { .opcode = 0x4A, .mode = Direct, .cycles = 2, .rmw = true }
) {
    auto D = operand();
    set_flag_if(CARRY_BIT, D & 0x1);
    D >>= 1;
    set_flags(D);
    operand(D);
}

OPCODE(ASL,
       { .opcode = 0x06, .mode = ZeroPage, .cycles = 5, .rmw = true },
       { .opcode = 0x0E, .mode = Absolute, .cycles = 6, .rmw = true },
       { .opcode = 0x16, .mode = ZeroPageX, .cycles = 6, .rmw = true },
       { .opcode = 0x1E, .mode = AbsoluteX, .cycles = 7, .rmw = true },
       { .opcode = 0x0A, .mode = Direct, .cycles = 2, .rmw = true }
) {
    auto D = operand();
    set_flag_if(CARRY_BIT, D & 0x80);
    D <<= 1;
    set_flags(D);
    operand(D);
}

OPCODE(ROR,
       { .opcode = 0x66, .mode = ZeroPage, .cycles = 5, .rmw = true },
       { .opcode = 0x6E, .mode = Absolute, .cycles = 6, .rmw = true },
       { .opcode = 0x76, .mode = ZeroPageX, .cycles = 6, .rmw = true },
       { .opcode = 0x7E, .mode = AbsoluteX, .cycles = 7, .rmw = true },
       { .opcode = 0x6A, .mode = Direct, .cycles = 2, .rmw = true }
) {
    auto D = operand();
    auto c = FLAG_CARRY;
    set_flag_if(CARRY_BIT, D & 0x1);
    D >>= 1;
    if (c) D |= 0x80;
    set_flags(D);
    operand(D);
}

OPCODE(ROL,
       { .opcode = 0x26, .mode = ZeroPage, .cycles = 5, .rmw = true },
       { .opcode = 0x2E, .mode = Absolute, .cycles = 6, .rmw = true },
       { .opcode = 0x36, .mode = ZeroPageX, .cycles = 6, .rmw = true },
       { .opcode = 0x3E, .mode = AbsoluteX, .cycles = 7, .rmw = true },
       { .opcode = 0x2A, .mode = Direct, .cycles = 2, .rmw = true }
) {
    auto D = operand();
    auto c = FLAG_CARRY;
    set_flag_if(CARRY_BIT, D & 0x80);
    D <<= 1;
    if (c) D |= 0x1;
    set_flags(D);
    operand(D);
}

OPCODE(INC,
       { .opcode = 0xE6, .mode = ZeroPage, .cycles = 5, .rmw = true },
       { .opcode = 0xEE, .mode = Absolute, .cycles = 6, .rmw = true },
       { .opcode = 0xF6, .mode = ZeroPageX, .cycles = 6, .rmw = true },
       { .opcode = 0xFE, .mode = AbsoluteX, .cycles = 7, .rmw = true }
) {
    uint8_t D = operand() + 1;
    set_flags(D);
    operand(D);
}

OPCODE(DEC,
       { .opcode = 0xC6, .mode = ZeroPage, .cycles = 5, .rmw = true },
       { .opcode = 0xCE, .mode = Absolute, .cycles = 3, .rmw = true },
       { .opcode = 0xD6, .mode = ZeroPageX, .cycles = 6, .rmw = true },
       { .opcode = 0xDE, .mode = AbsoluteX, .cycles = 7, .rmw = true }) {
    uint8_t D = operand() - 1;
    set_flags(D);
    operand(D);
}

#undef FLAG_NEGATIVE
#undef FLAG_OVERFLOW
#undef FLAG_BRK_SOURCE
#undef FLAG_DECIMAL_MODE
#undef FLAG_INT_DISABLED
#undef FLAG_ZERO
#undef FLAG_CARRY
#undef OPCODE

int main() {
    cpu cpu;
    cpu.step();
    return 0;
}