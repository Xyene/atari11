#include "cpu.h"

opcode_handler cpu::opcode_handlers[256];

const char *cpu::opcode_names[256];

opcode_def cpu::opcode_defs[256];

#define OPCODE(x, ...) static register_opcode_ opcode_##x##_def(#x, &cpu::op_##x, {__VA_ARGS__}); \
                        void cpu::op_##x()

#define FLAG_NEGATIVE       (P & NEGATIVE_BIT)
#define FLAG_OVERFLOW       (P & OVERFLOW_BIT)
#define FLAG_BRK_SOURCE     (P & BRK_SOURCE_BIT)
#define FLAG_DECIMAL_MODE   (P & DECIMAL_MODE_BIT)
#define FLAG_INT_DISABLED   (P & INT_DISABLED_BIT)
#define FLAG_ZERO           (P & ZERO_BIT)
#define FLAG_CARRY          (P & CARRY_BIT)

OPCODE(JSR, { .opcode = 0x20, .cycles = 6 }) {
    push16(PC + 1);
    PC = next16();
}

OPCODE(RTI, { .opcode = 0x40, .cycles = 6 }) {
    next8();
    P = pop8();
    PC = pop16();
}

OPCODE(RTS, { .opcode = 0x60, .cycles = 6 }) {
    next8();
    PC = pop16() + 1;
}

OPCODE(INY, { .opcode = 0xC8, .cycles = 2 }) {
    set_flags(++Y);
}

OPCODE(DEY, { .opcode = 0x88, .cycles = 2 }) {
    set_flags(--Y);
}

OPCODE(INX, { .opcode = 0xE8, .cycles = 2 }) {
    set_flags(++X);
}

OPCODE(DEX, { .opcode = 0xCA, .cycles = 2, .mode = RMW }) {
    set_flags(--X);
}

OPCODE(TAY, { .opcode = 0xA8, .cycles = 2 }) {
    set_flags(Y = A);
}

OPCODE(TYA, { .opcode = 0x98, .cycles = 2 }) {
    set_flags(A = Y);
}

OPCODE(TAX, { .opcode = 0xA8, .cycles = 2, .mode = RMW }) {
    set_flags(X = A);
}

OPCODE(TXA, { .opcode = 0x8A, .cycles = 2, .mode = RMW }) {
    set_flags(A = X);
}

OPCODE(TSX, { .opcode = 0xBA, .cycles = 2 }) {
    set_flags(X = SP);
}

OPCODE(TXS, { .opcode = 0x9A, .cycles = 2, .mode = RMW }) {
    set_flags(SP = X);
}

OPCODE(PHP, { .opcode = 0x08, .cycles = 3 }) {
    push8(P | BRK_SOURCE_BIT);
}

OPCODE(PLP, { .opcode = 0x28, .cycles = 4 }) {
    P = pop8() & ~BRK_SOURCE_BIT;
}

OPCODE(PLA, { .opcode = 0x68, .cycles = 4 }) {
    switch (A) {
        case 0 ... 5:
            A = pop8();
            break;
        case 6 ... 7:
            A = pop16();
            break;
    }
    set_flags(A = pop8());
}

OPCODE(PHA, { .opcode = 0x48, .cycles = 3 }) {
    push8(A);
}

OPCODE(BIT, { .opcode = 0x24, .cycles = 3, .mode = ZeroPage },
            { .opcode = 0x2C, .cycles = 4, .mode = Absolute }
) {
    auto val = operand();
    set_flag_if(OVERFLOW_BIT, val & 0x40);
    set_flag_if(ZERO_BIT, !(val & A));
    set_flag_if(NEGATIVE_BIT, val & 0x80);
}

void cpu::branch_if(bool cond) {
    uint16_t new_pc = PC + nexts8() + 1;
    if (cond) {
        PC = new_pc;
        cycle++;
    }
}

OPCODE(JMP, { .opcode = 0x4C, .cycles = 3},
            { .opcode = 0x6C, .cycles = 5}
) {
    if (current_instruction_ == 0x4C)
        PC = next16();
    else if (current_instruction_ == 0x6C) {
        uint16_t lo = next16();
        // AN INDIRECT JUMP MUST NEVER USE A VECTOR BEGINNING ON THE LAST BYTE OF A PAGE
        //
        // If address $3000 contains $40, $30FF contains $80, and $3100 contains $50,
        // the result of JMP ($30FF) will be a transfer of control to $4080 rather than
        // $5080 as you intended i.e. the 6502 took the low byte of the address from
        // $30FF and the high byte from $3000.
        //
        // http://www.6502.org/tutorials/6502opcodes.html
        uint16_t hi = (lo & 0xFF) == 0xFF ? lo - 0xFF : lo + 1;
        uint16_t old_PC = PC;
        PC = read8(lo) | (read8(hi) << 8);;

        if ((old_PC & 0xFF00) != (PC & 0xFF00)) cycle += 2;
    }
}

OPCODE(BCS, { .opcode = 0xB0, .cycles = 2 }) {
    branch_if(FLAG_CARRY);
}

OPCODE(BCC, { .opcode = 0x90, .cycles = 2 }) {
    branch_if(!FLAG_CARRY);
}

OPCODE(BEQ, { .opcode = 0xF0, .cycles = 2 }) {
    branch_if(FLAG_ZERO);
}

OPCODE(BNE, { .opcode = 0xD0, .cycles = 2 }) {
    branch_if(!FLAG_ZERO);
}

OPCODE(BVS, { .opcode = 0x70, .cycles = 2 }) {
    branch_if(FLAG_OVERFLOW);
}

OPCODE(BVC, { .opcode = 0x50, .cycles = 2 }) {
    branch_if(!FLAG_OVERFLOW);
}

OPCODE(BPL, { .opcode = 0x10, .cycles = 2 }) {
    branch_if(!FLAG_NEGATIVE);
}

OPCODE(BMI, { .opcode = 0x30, .cycles = 2 }) {
    branch_if(FLAG_NEGATIVE);
}

OPCODE(STA, { .opcode = 0x81, .cycles = 6, .mode = IndirectX },
            { .opcode = 0x91, .cycles = 6, .mode = IndirectY },
            { .opcode = 0x95, .cycles = 4, .mode = ZeroPageX },
            { .opcode = 0x99, .cycles = 5, .mode = AbsoluteY },
            { .opcode = 0x9D, .cycles = 5, .mode = AbsoluteX },
            { .opcode = 0x85, .cycles = 3, .mode = ZeroPage },
            { .opcode = 0x8D, .cycles = 4, .mode = Absolute }
) {
    operand(A);
}

OPCODE(STX, { .opcode = 0x96, .cycles = 4, .mode = ZeroPageY },
            { .opcode = 0x86, .cycles = 3, .mode = ZeroPage },
            { .opcode = 0x8E, .cycles = 4, .mode = Absolute }
) {
    operand(X);
}

OPCODE(STY, { .opcode = 0x94, .cycles = 4, .mode = ZeroPageX },
            { .opcode = 0x84, .cycles = 3, .mode = ZeroPage },
            { .opcode = 0x8C, .cycles = 4, .mode = Absolute }
) {
    operand(Y);
}

OPCODE(CLC, { .opcode = 0x18, .cycles = 2 }) {
    set_flag_if(CARRY_BIT, false);
}

OPCODE(SEC, { .opcode = 0x38, .cycles = 2 }) {
    set_flag_if(CARRY_BIT, true);
}

OPCODE(CLI, { .opcode = 0x58, .cycles = 2 }) {
    set_flag_if(INT_DISABLED_BIT, false);
}

OPCODE(SEI, { .opcode = 0x78, .cycles = 2 }) {
    set_flag_if(INT_DISABLED_BIT, true);
}

OPCODE(CLV, { .opcode = 0xB8, .cycles = 2 }) {
    set_flag_if(OVERFLOW_BIT, false);
}

OPCODE(CLD, { .opcode = 0xD8, .cycles = 2 }) {
    set_flag_if(DECIMAL_MODE_BIT, false);
}

OPCODE(SED, { .opcode = 0xF8, .cycles = 2 }) {
    set_flag_if(DECIMAL_MODE_BIT, true);
}

OPCODE(NOP, { .opcode = 0xEA, .cycles = 2 }) {
    // Do nothing
}

OPCODE(LDA, { .opcode = 0xA1, .cycles = 6, .mode = IndirectX },
            { .opcode = 0xA5, .cycles = 3, .mode = ZeroPage },
            { .opcode = 0xA9, .cycles = 2, .mode = Immediate },
            { .opcode = 0xAD, .cycles = 4, .mode = Absolute },
            { .opcode = 0xB1, .cycles = 5, .mode = IndirectY | PageBoundary },
            { .opcode = 0xB5, .cycles = 4, .mode = ZeroPageX },
            { .opcode = 0xB9, .cycles = 4, .mode = AbsoluteY | PageBoundary },
            { .opcode = 0xBD, .cycles = 4, .mode = AbsoluteX | PageBoundary }
) {
    set_flags(A = operand());
}

OPCODE(LDY, { .opcode = 0xA0, .cycles = 2, .mode = Immediate },
            { .opcode = 0xA4, .cycles = 3, .mode = ZeroPage },
            { .opcode = 0xAC, .cycles = 4, .mode = Absolute },
            { .opcode = 0xB4, .cycles = 4, .mode = ZeroPageX },
            { .opcode = 0xBC, .cycles = 4, .mode = AbsoluteX | PageBoundary }
) {
    set_flags(Y = operand());
}

OPCODE(LDX, { .opcode = 0xA2, .cycles = 2, .mode = Immediate | RMW },
            { .opcode = 0xA6, .cycles = 3, .mode = ZeroPage | RMW },
            { .opcode = 0xAE, .cycles = 4, .mode = Absolute | RMW },
            { .opcode = 0xB6, .cycles = 4, .mode = ZeroPageY | RMW },
            { .opcode = 0xBE, .cycles = 4, .mode = AbsoluteY | RMW | PageBoundary }
) {
    set_flags(X = operand());
}

OPCODE(ORA, { .opcode = 0x01, .cycles = 6, .mode = IndirectX },
            { .opcode = 0x05, .cycles = 3, .mode = ZeroPage },
            { .opcode = 0x09, .cycles = 2, .mode = Immediate },
            { .opcode = 0x0D, .cycles = 4, .mode = Absolute },
            { .opcode = 0x11, .cycles = 5, .mode = IndirectY | PageBoundary },
            { .opcode = 0x15, .cycles = 4, .mode = ZeroPageX },
            { .opcode = 0x19, .cycles = 4, .mode = AbsoluteY | PageBoundary },
            { .opcode = 0x1D, .cycles = 4, .mode = AbsoluteX | PageBoundary }
) {
    set_flags(A |= operand());
}

OPCODE(AND, { .opcode = 0x21, .cycles = 6, .mode = IndirectX },
            { .opcode = 0x25, .cycles = 3, .mode = ZeroPage },
            { .opcode = 0x29, .cycles = 2, .mode = Immediate },
            { .opcode = 0x2D, .cycles = 4, .mode = Absolute },
            { .opcode = 0x31, .cycles = 5, .mode = IndirectY | PageBoundary },
            { .opcode = 0x35, .cycles = 4, .mode = ZeroPageX },
            { .opcode = 0x39, .cycles = 4, .mode = AbsoluteY | PageBoundary },
            { .opcode = 0x3D, .cycles = 4, .mode = AbsoluteX | PageBoundary }
) {
    set_flags(A &= operand());
}

OPCODE(EOR, { .opcode = 0x41, .cycles = 6, .mode = IndirectX },
            { .opcode = 0x45, .cycles = 3, .mode = ZeroPage },
            { .opcode = 0x49, .cycles = 2, .mode = Immediate },
            { .opcode = 0x4D, .cycles = 4, .mode = Absolute },
            { .opcode = 0x51, .cycles = 5, .mode = IndirectY | PageBoundary },
            { .opcode = 0x55, .cycles = 4, .mode = ZeroPageX },
            { .opcode = 0x59, .cycles = 4, .mode = AbsoluteY | PageBoundary },
            { .opcode = 0x5D, .cycles = 4, .mode = AbsoluteX | PageBoundary }
) {
    set_flags(A ^= operand());
}

void cpu::ADC_regular(int8_t val) {
    if (FLAG_DECIMAL_MODE) throw std::logic_error("BCD not implemented");
    int nA = (int8_t)A + val + (FLAG_CARRY ? 1 : 0);
    set_flag_if(OVERFLOW_BIT, nA < -128 || nA > 127);
    set_flag_if(CARRY_BIT, (A + val + (FLAG_CARRY ? 1 : 0)) > 0xFF);
    A = nA;
}

OPCODE(SBC, { .opcode = 0xE1, .cycles = 6, .mode = IndirectX },
            { .opcode = 0xE5, .cycles = 3, .mode = ZeroPage },
            { .opcode = 0x69, .cycles = 2, .mode = Immediate },
            { .opcode = 0xE9, .cycles = 2, .mode = Immediate }, // Official duplicate of $69
            { .opcode = 0xEB, .cycles = 2, .mode = Immediate }, // Unofficial duplicate of $69
            { .opcode = 0xED, .cycles = 4, .mode = Absolute },
            { .opcode = 0xF1, .cycles = 5, .mode = IndirectY | PageBoundary },
            { .opcode = 0xF5, .cycles = 4, .mode = ZeroPageX },
            { .opcode = 0xF9, .cycles = 4, .mode = AbsoluteY | PageBoundary },
            { .opcode = 0xFD, .cycles = 4, .mode = AbsoluteX | PageBoundary }
) { ADC_regular(~operand()); }

OPCODE(ADC, { .opcode = 0x61, .cycles = 6, .mode = IndirectX, },
            { .opcode = 0x65, .cycles = 3, .mode = ZeroPage },
            { .opcode = 0x69, .cycles = 2, .mode = Immediate, },
            { .opcode = 0x6D, .cycles = 4, .mode = Absolute },
            { .opcode = 0x71, .cycles = 5, .mode = IndirectY | PageBoundary },
            { .opcode = 0x75, .cycles = 4, .mode = ZeroPageX, },
            { .opcode = 0x79, .cycles = 4, .mode = AbsoluteY | PageBoundary },
            { .opcode = 0x7D, .cycles = 4, .mode = AbsoluteX | PageBoundary }
) { ADC_regular(operand()); }

OPCODE(BRK,{ .opcode = 0x00, .cycles = 7 }) {
    next8();
    push8(P | BRK_SOURCE_BIT);
    set_flag_if(INT_DISABLED_BIT, true);
    PC = read16(0xFFFE);
}

void cpu::compare(uint8_t val) {

}

OPCODE(CMP, { .opcode = 0xC1, .cycles = 6, .mode = IndirectX },
            { .opcode = 0xC5, .cycles = 3, .mode = ZeroPage },
            { .opcode = 0xC9, .cycles = 2, .mode = Immediate },
            { .opcode = 0xCD, .cycles = 4, .mode = Absolute },
            { .opcode = 0xD1, .cycles = 5, .mode = IndirectY | PageBoundary },
            { .opcode = 0xD5, .cycles = 4, .mode = ZeroPageX },
            { .opcode = 0xD9, .cycles = 4, .mode = AbsoluteY | PageBoundary },
            { .opcode = 0xDD, .cycles = 4, .mode = AbsoluteX | PageBoundary }
) {
    compare(A);
}

OPCODE(CPX, { .opcode = 0xE0, .cycles = 2, .mode = Immediate },
            { .opcode = 0xE4, .cycles = 3, .mode = ZeroPage },
            { .opcode = 0xEC, .cycles = 4, .mode = Absolute }
) {
    compare(X);
}

OPCODE(CPY, { .opcode = 0xC0, .cycles = 2, .mode = Immediate },
            { .opcode = 0xC4, .cycles = 3, .mode = ZeroPage },
            { .opcode = 0xCC, .cycles = 4, .mode = Absolute }
) {
    compare(Y);
}

OPCODE(LSR, { .opcode = 0x46, .cycles = 5, .mode = ZeroPage | RMW },
            { .opcode = 0x4E, .cycles = 6, .mode = Absolute | RMW },
            { .opcode = 0x56, .cycles = 6, .mode = ZeroPageX | RMW },
            { .opcode = 0x5E, .cycles = 7, .mode = AbsoluteX | RMW },
            { .opcode = 0x4A, .cycles = 2, .mode = Direct | RMW }
) {
    auto D = operand();
    set_flag_if(CARRY_BIT, D & 0x1);
    D >>= 1;
    set_flags(D);
    operand(D);
}

OPCODE(ASL, { .opcode = 0x06, .cycles = 5, .mode = ZeroPage | RMW },
            { .opcode = 0x0E, .cycles = 6, .mode = Absolute | RMW },
            { .opcode = 0x16, .cycles = 6, .mode = ZeroPageX | RMW },
            { .opcode = 0x1E, .cycles = 7, .mode = AbsoluteX | RMW },
            { .opcode = 0x0A, .cycles = 2, .mode = Direct | RMW }
) {
    auto D = operand();
    set_flag_if(CARRY_BIT, D & 0x80);
    D <<= 1;
    set_flags(D);
    operand(D);
}

OPCODE(ROR, { .opcode = 0x66, .cycles = 5, .mode = ZeroPage | RMW },
            { .opcode = 0x6E, .cycles = 6, .mode = Absolute | RMW },
            { .opcode = 0x76, .cycles = 6, .mode = ZeroPageX | RMW },
            { .opcode = 0x7E, .cycles = 7, .mode = AbsoluteX | RMW },
            { .opcode = 0x6A, .cycles = 2, .mode = Direct | RMW }
) {
    auto D = operand();
    auto c = FLAG_CARRY;
    set_flag_if(CARRY_BIT, D & 0x1);
    D >>= 1;
    if (c) D |= 0x80;
    set_flags(D);
    operand(D);
}

OPCODE(ROL, { .opcode = 0x26, .cycles = 5, .mode = ZeroPage | RMW },
            { .opcode = 0x2E, .cycles = 6, .mode = Absolute | RMW },
            { .opcode = 0x36, .cycles = 6, .mode = ZeroPageX | RMW },
            { .opcode = 0x3E, .cycles = 7, .mode = AbsoluteX | RMW },
            { .opcode = 0x2A, .cycles = 2, .mode = Direct | RMW }
) {
    auto D = operand();
    auto c = FLAG_CARRY;
    set_flag_if(CARRY_BIT, D & 0x80);
    D <<= 1;
    if (c) D |= 0x1;
    set_flags(D);
    operand(D);
}

OPCODE(INC, { .opcode = 0xE6, .cycles = 5, .mode = ZeroPage | RMW },
            { .opcode = 0xEE, .cycles = 6, .mode = Absolute | RMW },
            { .opcode = 0xF6, .cycles = 6, .mode = ZeroPageX | RMW },
            { .opcode = 0xFE, .cycles = 7, .mode = AbsoluteX | RMW }
) {
    auto D = operand() + 1;
    set_flags(D);
    operand(D);
}

OPCODE(DEC, { .opcode = 0xC6, .cycles = 5, .mode = ZeroPage | RMW },
            { .opcode = 0xCE, .cycles = 3, .mode = Absolute | RMW },
            { .opcode = 0xD6, .cycles = 6, .mode = ZeroPageX | RMW },
            { .opcode = 0xDE, .cycles = 7, .mode = AbsoluteX | RMW }) {
    auto D = operand() - 1;
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
