#include "cpu.h"
#include "pia.h"
#include "tia.h"
#include "cartridge.h"

int main() {
    cpu cpu;
    tia tia(cpu);
    pia pia(cpu);
    cartridge cart(cpu, "SPCINVAD.BIN");
    printf("Ready to begin\n");
    cpu.reset();
    cpu.step();
    cpu.step();
    cpu.step();
    cpu.step();
    return 0;
}