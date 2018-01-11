//
// Created by Tudor Brindus on 2018-01-10.
//

#ifndef INC_2600_CPU_H
#define INC_2600_CPU_H

#include <cstdint>
#include <initializer_list>
#include <iostream>

#define    CARRY_BIT            0x1
#define    ZERO_BIT             0x2
#define    INT_DISABLED_BIT     0x4
#define    DECIMAL_MODE_BIT     0x8
#define    BRK_SOURCE_BIT       0x10
#define    OVERFLOW_BIT         0x40
#define    NEGATIVE_BIT         0x80

class cpu;

typedef void (cpu::* opcode_handler)();

enum opcode_type {
    Default,
    RMW,
    Page,
    Page_RMW
};

enum address_mode {
    None,
    Direct,
    Immediate,
    ZeroPage,
    Absolute,
    ZeroPageX,
    ZeroPageY,
    AbsoluteX,
    AbsoluteY,
    IndirectX,
    IndirectY
};

struct opcode_def {
    uint8_t opcode;
    uint8_t cycles;
    address_mode mode;
    bool rmw;
    bool page_boundary;
};

class cpu {
    friend class _register_opcode;

private:
    uint8_t A;
    uint8_t X;
    uint8_t Y;
    uint8_t SP;
    uint16_t PC;
    uint8_t P;
    uint32_t cycle;

    static opcode_handler opcode_handlers[256];
    static std::string opcode_names[256];
    static opcode_def opcode_defs[256];

public:
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

    void set_flags(uint8_t val);

    void set_flag_if(uint8_t mask, bool cond);

    void push16(uint16_t val);

    void push8(uint8_t val);

    uint8_t pop8();

    uint16_t pop16();

    uint16_t next16();

    uint8_t next8();

    int8_t nexts8();

    void branch_if(bool cond);

    void compare(uint8_t val);
};

class _register_opcode {
public:
    _register_opcode(const std::string &name, opcode_handler handler, std::initializer_list<opcode_def> ops) {
        for (auto op : ops) {
            cpu::opcode_handlers[op.opcode] = handler;
            cpu::opcode_names[op.opcode] = name;
            cpu::opcode_defs[op.opcode] = op;
            std::cout << (int) op.opcode << " (" << name << "[" << "mode=" << op.mode << ", rmw=" << op.rmw << "])" << std::endl;
        }
    }
};

#endif //INC_2600_CPU_H
