#include "cpu.h"
#include "kernel/riscv.h"

struct cpu cpus[NCPU];

int cpuid()
{
    int id = r_tp();
    return id;
}

struct cpu * mycpu(void)
{
    int id = cpuid();
    struct cpu *c = &cpus[id];
    return c;
}