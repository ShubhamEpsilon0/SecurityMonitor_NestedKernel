#include "kernel/types.h"
#include "kernel/riscv.h"
#include "trapframe.h"
#include "kernel/defs.h"
#include "kernel/memlayout.h"

void main();
void sm_init();
void timerinit();

struct sm_trapframe *trapframe = (struct sm_trapframe *)SM_TRAPFRAME;
__attribute__((aligned(16))) char sm_stack[4096];
extern char sm_trampoline[], machinevec[], machineret[];
extern void machinetrapret();

// entry.S jumps here in machine mode on stack0.
void start()
{
    // memset trapframe
    memset(trapframe, 0, sizeof(struct sm_trapframe));

    // set M Previous Privilege mode to Supervisor, for mret.
    unsigned long x = r_mstatus();
    x &= ~MSTATUS_MPP_MASK;
    x |= MSTATUS_MPP_S;
    w_mstatus(x);

    // disable paging for now.
    w_satp(0);

    //setup memory regions
    w_pmpaddr0(0x80000000 >> 2);
    w_pmpaddr1(0x80F00000 >> 2);
    w_pmpcfg0(r_pmpcfg0() | 0x080f);

    // set M Exception Program Counter to main, for mret.
    // requires gcc -mcmodel=medany
    trapframe->epc = (uint64)loadKernel();

    // delegate all interrupts and exceptions to supervisor mode.
    // TODO: delegate ECalls from supervisor mode to machine mode.
    w_medeleg(0xffff & ~(1 << 9)); // To delegate ecall in smode to machine mode. & ~(1 << 9)
    w_medeleg(r_medeleg() & ~(0xA2));
    w_mideleg(0xffff);
    w_sie(r_sie() | SIE_SEIE | SIE_STIE | SIE_SSIE);

    // ask for clock interrupts.
    timerinit();

    // keep each CPU's hartid in its tp register, for cpuid().
    int id = r_mhartid();
    w_tp(id);

    sm_init();

    // switch to supervisor mode and jump to main().
    machinetrapret();
}

// ask each hart to generate timer interrupts.
void timerinit()
{
    // enable supervisor-mode timer interrupts.
    w_mie(r_mie() | MIE_STIE);

    // enable the sstc extension (i.e. stimecmp).
    w_menvcfg(r_menvcfg() | (1L << 63));

    // allow supervisor to use stimecmp and time.
    w_mcounteren(r_mcounteren() | 2);

    // ask for the very first timer interrupt.
    w_stimecmp(r_time() + 1000000);
}


void sm_init () {
    consoleinit();
    printfinit();
    printf("\n");
    printf("Initializing SVM\n");
}