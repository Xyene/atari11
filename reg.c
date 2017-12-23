#include <stdint.h>
#include <stdbool.h>
#include "reg.h"

/*
struct reg8 {
	uint8_t x;
};

struct reg16 {
	uint16_t x;
};
*/

typedef uint8_t reg8;
typedef uint16_t reg16;

struct cpu_state {
	bool negative;
	bool overflow;
	bool break_source;
	bool decimal_mode;
	bool interrupts_disabled;
	bool zero;
	bool carry;
};

struct cpu {
	reg8 A;
	reg8 X;
	reg8 Y;
	reg16 PC;
	reg16 SP;
	reg8 P;
	uint32_t cycle;
};

#define OPCODE(x) inline void op_##x(struct cpu *cpu)

#define FLAG_NEGATIVE		(P & NEGATIVE_BIT)
#define FLAG_OVERFLOW		(P & OVERFLOW_BIT)
#define FLAG_BRK_SOURCE		(P & BRK_SOURCE_BIT)
#define FLAG_DECIMAL_MODE	(P & DECIMAL_MODE_BIT)
#define FLAG_INT_DISABLED	(P & INT_DISABLED_BIT)
#define FLAG_ZERO		(P & ZERO_BIT)
#define FLAG_CARRY		(P & CARRY_BIT)

#define A	cpu->A
#define X	cpu->X
#define Y	cpu->Y
#define PC	cpu->PC
#define SP	cpu->SP
#define P	cpu->P

#define PUSH16(val) push_word(cpu, val)

#define POP16() pop_word(cpu)

#define PUSH8(val) push_byte(cpu, val)

#define POP8() pop_byte(cpu)

#define NEXT16() next_word(cpu)

#define NEXT8() next_byte(cpu)

#define SET_FLAG(mask, cond) set_flag(cpu, mask, cond)

#define READ() 0

#define READ16(addr) addr

#define WRITE(val) val

void set_flag(struct cpu *cpu, uint8_t mask, bool cond) {
	//cpu->P = cond ? cpu->P | 1 : cpu->P & ~1;
} 

void push_word(struct cpu *cpu, uint16_t val) {
	// TODO
}

void push_byte(struct cpu *cpu, uint8_t val) {
	// TODO
}

uint8_t pop_byte(struct cpu *cpu) {
	return 0;
}

uint16_t pop_word(struct cpu *cpu) {
	return 0;
}

uint16_t next_word(struct cpu *cpu) {
	return 0;
}

uint8_t next_byte(struct cpu *cpu) {
	return 0;
}

int8_t next_signed_byte(struct cpu *cpu) {
	return 0;
}

void branch_if(struct cpu *cpu, bool cond) {
	uint8_t new_pc = (uint8_t)(PC + next_signed_byte(cpu) + 1);
	if (cond) {
		PC = new_pc;
		cpu->cycle++;
	}
}

OPCODE(JSR)
{
	PUSH16(PC + 1);
	PC = NEXT16();
}

OPCODE(RTI)
{
	NEXT8();
	P = POP8();
	PC = POP16();
}

OPCODE(RTS)
{
	NEXT8();
	PC = POP16() + 1;
}

OPCODE(INY) { Y++; }

OPCODE(DEY) { Y--; }

OPCODE(INX) { X++; }

OPCODE(DEX) { X--; }

OPCODE(TAY) { Y = A; }

OPCODE(TYA) { A = Y; }

OPCODE(TAX) { A = X; }

OPCODE(TXA) { X = Y; }

OPCODE(TSX) { X = SP; }

OPCODE(TXS) { SP = X; }

OPCODE(PHP) { PUSH8(P | BRK_SOURCE_BIT); }

OPCODE(PLP) { P = POP8() & ~BRK_SOURCE_BIT; }

OPCODE(PLA) { A = POP8(); }

OPCODE(PHA) { PUSH8(A); }

OPCODE(BIT)
{
	uint8_t val = READ();
	SET_FLAG(OVERFLOW_BIT, val & 0x40);
	SET_FLAG(ZERO_BIT, !(val & A));
	SET_FLAG(NEGATIVE_BIT, val & 0x80);
}

OPCODE(BCS) { BRANCH_IF(FLAG_CARRY); }

OPCODE(BCC) { BRANCH_IF(!FLAG_CARRY); }

OPCODE(BEQ) { BRANCH_IF(FLAG_ZERO); }

OPCODE(BNE) { BRANCH_IF(!FLAG_ZERO); }

OPCODE(BVS) { BRANCH_IF(FLAG_OVERFLOW); }

OPCODE(BVC) { BRANCH_IF(!FLAG_OVERFLOW); }

OPCODE(BPL) { BRANCH_IF(!FLAG_NEGATIVE); }

OPCODE(BMI) { BRANCH_IF(FLAG_NEGATIVE); }

OPCODE(STA) { WRITE(A); }

OPCODE(STX) { WRITE(X); }

OPCODE(STY) { WRITE(Y); }

OPCODE(CLC) { SET_FLAG(CARRY_BIT, false); }

OPCODE(SEC) { SET_FLAG(CARRY_BIT, true); }

OPCODE(CLI) { SET_FLAG(INT_DISABLED_BIT, false); }

OPCODE(SEI) { SET_FLAG(INT_DISABLED_BIT, true); }

OPCODE(CLV) { SET_FLAG(OVERFLOW_BIT, false); }

OPCODE(CLD) { SET_FLAG(DECIMAL_MODE_BIT, false); }

OPCODE(SED) { SET_FLAG(DECIMAL_MODE_BIT, true); }

OPCODE(NOP) { }

OPCODE(LDA) { A = READ(); }

OPCODE(LDY) { Y = READ(); }

OPCODE(LDX) { X = READ(); }

OPCODE(ORA) { A |= READ(); }

OPCODE(AND) { A &= READ(); }

OPCODE(EOR) { A ^= READ(); }

OPCODE(SBC) { /* TODO */ }

OPCODE(ADC) { /* TODO */ }

OPCODE(BRK)
{
	NEXT8();
	PUSH8(P | BRK_SOURCE_BIT);
	SET_FLAG(INT_DISABLED_BIT, true);
	PC = READ16(0xFFFE);
}

OPCODE(CMP) { COMPARE(A); }

OPCODE(CPX) { COMPARE(X); }

OPCODE(CPY) { COMPARE(Y); }

OPCODE(LSR)
{
	uint8_t D = READ();
	SET_FLAG(CARRY_BIT, D & 0x1);
	D >>= 1;
	SET_FLAGS(D);
	WRITE(D);
}

OPCODE(ASL)
{
	uint8_t D = READ();
	SET_FLAG(CARRY_BIT, D & 0x80);
	D <<= 1;
	SET_FLAGS(D);
	WRITE(D);
}

OPCODE(ROR)
{
	uint8_t D = READ();
	bool c = FLAG_CARRY;
	SET_FLAG(CARRY_BIT, D & 0x1);
	D >>= 1;
	if (c) D |= 0x80;
	SET_FLAGS(D);
	WRITE(D);
}

OPCODE(ROL)
{
	uint8_t D = READ();
	bool c = FLAG_CARRY;
	SET_FLAG(CARRY_BIT, D & 0x80);
	D <<= 1;
	if (c) D |= 0x1;
	SET_FLAGS(D);
	WRITE(D);
}

OPCODE(INC)
{
	uint8_t D = READ() + 1;
	SET_FLAGS(D);
	WRITE(D);
}

OPCODE(DEC)
{
	uint8_t D = READ() + 1;
	SET_FLAGS(D);
	WRITE(D);
}

int main() {
	return 0;
}

