#ifndef INC_2600_CPU_H
#define INC_2600_CPU_H

#include <cstdint>
#include <initializer_list>
#include <functional>
#include <iostream>
#include <string>

#include "addressable.h"

#define    CARRY_BIT            0x1
#define    ZERO_BIT             0x2
#define    INT_DISABLED_BIT     0x4
#define    DECIMAL_MODE_BIT     0x8
#define    BRK_SOURCE_BIT       0x10
#define    OVERFLOW_BIT         0x40
#define    NEGATIVE_BIT         0x80

class cpu;

typedef void (cpu::* opcode_handler)();

enum address_mode {
    None = 0x00,
    Direct = 0x1,
    Immediate = 0x2,
    ZeroPage = 0x4,
    Absolute = 0x10,
    ZeroPageX = 0x20,
    ZeroPageY = 0x40,
    AbsoluteX = 0x80,
    AbsoluteY = 0x100,
    IndirectX = 0x200,
    IndirectY = 0x400,
    RMW = 0x800,
    PageBoundary = 0x1000
};

struct opcode_def {
    uint8_t opcode;
    uint8_t cycles;
    int mode;

    inline address_mode get_address_mode() { return (address_mode)(mode & (RMW - 1)); };

    inline bool is_rmw() { return (mode & RMW) > 0; }

    inline bool has_extra_page_boundary_cycle() { return (mode & PageBoundary) > 0; }
};

class cpu : addressable<8192> {
    friend class register_opcode_;

private:
    uint8_t A;
    uint8_t X;
    uint8_t Y;
    uint8_t SP;
    uint16_t PC;
    uint8_t P;
    uint32_t cycle;

    uint8_t current_instruction_;

    static opcode_handler opcode_handlers[256];
    static const char* opcode_names[256];
    static opcode_def opcode_defs[256];

public:
    cpu();

    void step();

    void reset();

#define OPCODE(x) void op_##x()

    OPCODE(JSR);

    OPCODE(RTI);

    OPCODE(RTS);

    OPCODE(INY);

    OPCODE(DEY);

    OPCODE(INX);

    OPCODE(DEX);

    OPCODE(TAY);

    OPCODE(TYA);

    OPCODE(TAX);

    OPCODE(TXA);

    OPCODE(TSX);

    OPCODE(TXS);

    OPCODE(PHP);

    OPCODE(PLP);

    OPCODE(PLA);

    OPCODE(PHA);

    OPCODE(BIT);

    OPCODE(BCS);

    OPCODE(BCC);

    OPCODE(BEQ);

    OPCODE(BNE);

    OPCODE(BVS);

    OPCODE(BVC);

    OPCODE(BPL);

    OPCODE(BMI);

    OPCODE(STA);

    OPCODE(STX);

    OPCODE(STY);

    OPCODE(CLC);

    OPCODE(SEC);

    OPCODE(CLI);

    OPCODE(SEI);

    OPCODE(CLV);

    OPCODE(CLD);

    OPCODE(SED);

    OPCODE(NOP);

    OPCODE(LDA);

    OPCODE(LDY);

    OPCODE(LDX);

    OPCODE(ORA);

    OPCODE(AND);

    OPCODE(EOR);

    OPCODE(SBC);

    OPCODE(ADC);

    OPCODE(BRK);

    OPCODE(CMP);

    OPCODE(CPX);

    OPCODE(CPY);

    OPCODE(LSR);

    OPCODE(ASL);

    OPCODE(ROR);

    OPCODE(ROL);

    OPCODE(INC);

    OPCODE(DEC);

#undef OPCODE

private:
    uint8_t operand() const;

    void operand(uint8_t val);

    uint8_t read8(uint16_t addr) const;

    uint16_t read16(uint16_t addr) const;

    void write8(uint16_t addr, uint8_t val);

    void write16(uint16_t addr, uint16_t val);

    inline void set_flags(uint8_t val) {
        set_flag_if(ZERO_BIT, !val);
        set_flag_if(NEGATIVE_BIT, val < 0);
    }

    inline void set_flag_if(uint8_t mask, bool cond) {
        if (cond) P |= mask; else P &= ~mask;
        // P = (P & ~mask) | (-cond & mask);
    }

    void push16(uint16_t val);

    void push8(uint8_t val);

    uint8_t pop8();

    uint16_t pop16();

    uint16_t next16();

    uint8_t next8();

    int8_t nexts8();

    inline void branch_if(bool cond);

    inline void compare(uint8_t val);
};

class register_opcode_ {
public:
    register_opcode_(const char *name, opcode_handler handler, std::initializer_list<opcode_def> ops) {
        for (auto op : ops) {
            cpu::opcode_handlers[op.opcode] = handler;
            cpu::opcode_names[op.opcode] = name;
            cpu::opcode_defs[op.opcode] = op;
            std::cout << (int) op.opcode << " (" << name << "[" << "mode=" << op.mode << ", rmw="
                      << op.is_rmw() << ", page_cycle=" << op.has_extra_page_boundary_cycle() << "])" << std::endl;
        }
    }
};

#endif //INC_2600_CPU_H
