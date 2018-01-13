#include "cpu.h"
#include "pia.h"
#include "tia.h"
#include "cartridge.h"

int main() {
    cpu cpu;
    tia tia(cpu);
    pia pia(cpu);
    cartridge cartridge(cpu, "SPCINVAD.BIN");
    printf("Ready to begin\n");
    cpu.reset();

    for(int i = 0; i < 1000; i++) cpu.step();

    return 0;
}