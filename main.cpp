#include "cpu.h"
#include "pia.h"
#include "tia.h"

int main() {
    cpu cpu;
    tia tia(cpu);
    pia pia(cpu);
    //cpu.step();
    return 0;
}