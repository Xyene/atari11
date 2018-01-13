#include "cpu.h"
#include "pia.h"
#include "tia.h"

int main() {
    cpu cpu;
    tia tia(cpu);
    pia pia(cpu);
    addressable<0x1000> cart;
    //cpu.step();
    return 0;
}