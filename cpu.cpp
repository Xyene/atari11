#include "cpu.h"

cpu::cpu() {
    reset();
    map_write_handler(0, 0x2000, [this](uint16_t addr, uint8_t val) {
        std::cout << addr << std::endl;
    });
    map_read_handler(0, 0x2000, [this](uint16_t addr) -> uint8_t {
        return 0;
    });
}

void cpu::step() {
    current_instruction_ = next8();
    fetched_current_addr_ = false;

    auto handler = opcode_handlers[current_instruction_];

    if (handler == nullptr) {
        printf("Invalid opcode %0X\n", current_instruction_);
        return;
    }

    cycle += opcode_defs[current_instruction_].cycles;

    (this->*handler)();
}

void cpu::reset() {
    A = 0;
    X = 0;
    Y = 0;
    SP = 0xFD;
    PC = 0;
    P = 0x24;
    cycle = 0;
}

uint8_t cpu::operand() {
    if (opcode_defs[current_instruction_].address_mode() == Direct)
        rmw_value_ = A;
    else
        rmw_value_ = read8(operand_address());
    return rmw_value_;
}

void cpu::operand(uint8_t val) {
    auto def = opcode_defs[current_instruction_];
    if (def.address_mode() == Direct)
        A = val;
    else {
        auto addr = operand_address();
        // Fake another write for RMW opcodes
        if (def.is_rmw())
            write8(addr, rmw_value_);
        write8(addr, val);
    }
}

uint8_t cpu::read8(uint16_t addr) const {
    return read_handlers[addr](addr);
}

uint16_t cpu::read16(uint16_t addr) const {
    return read8(addr) | (read8(addr + 1) << 8);
}

void cpu::write8(uint16_t addr, uint8_t val) {
    write_handlers[addr](addr, val);
}

void cpu::write16(uint16_t addr, uint16_t val) {
    write_handlers[addr](addr, val & 0xFF);
    write_handlers[addr](addr + 1, (val >> 8) & 0xFF);
}

uint8_t cpu::next8() {
    return read8(PC++);
}

uint16_t cpu::next16() {
    return next8() | (next8() << 8);
}

int8_t cpu::nexts8() {
    return next8();
}

void cpu::push8(uint8_t val) {
    write8(0x100 + SP, val);
    SP--;
}

uint8_t cpu::pop8() {
    SP++;
    return read8(0x100 + SP);
}

void cpu::push16(uint16_t val) {
    push8(val >> 8);
    push8(val & 0xFF);
}

uint16_t cpu::pop16() {
    return pop8() | (pop8() << 8);
}

uint16_t cpu::operand_address() {
    if (fetched_current_addr_)
        return current_addr_;

    fetched_current_addr_ = true;
    auto def = opcode_defs[current_instruction_];

    switch (def.address_mode()) {
        case Immediate:
            return PC++;
        case ZeroPage:
            return next8();
        case Absolute:
            return next16();
        case ZeroPageX:
            return (next8() + X) & 0xFF;
        case ZeroPageY:
            return (next8() + Y) & 0xFF;
        case AbsoluteX: {
            uint16_t addr = next16();
            if (def.has_extra_page_boundary_cycle() && (addr & 0xFF00) != ((addr + X) & 0xFF00)) cycle += 1;
            return addr + X;
        }
        case AbsoluteY: {
            uint16_t addr = next16();
            if (def.has_extra_page_boundary_cycle() && (addr & 0xFF00) != ((addr + Y) & 0xFF00)) cycle += 1;
            return addr + Y;
        }
        case IndirectX:
            return read16(next8() + X);
        case IndirectY: {
            uint16_t addr = read16(next8());
            if (def.has_extra_page_boundary_cycle() && (addr & 0xFF00) != ((addr + Y) & 0xFF00)) cycle += 1;
            return addr + Y;
        }
        default:
            throw std::logic_error("invalid address mode");
    }
}
