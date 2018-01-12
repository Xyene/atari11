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

uint8_t cpu::operand() const {
    return 0;
}

void cpu::operand(uint8_t val) {

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
