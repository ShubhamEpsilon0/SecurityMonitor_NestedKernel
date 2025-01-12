#include "trapframe.h"
#include "kernel/defs.h"
#include "kernel/riscv.h"
#include "kernel/types.h"

extern char sm_trampoline[], machinevec[], machineret[];

void machinetrapret();

void machinetrap()
{
    if (!(r_mstatus() & MSTATUS_MPP_S))
        //panic("machinetrap: not from Supervisor Mode");
        for (;;)
            ;

    w_mtvec((uint64)0); // Let it crash on Exception Todo: fix it

    trapframe->epc = r_mepc();

    if (r_mcause() == 9)
    {
        // system call
        trapframe->epc += 4;
        syscall();
    }
    else if (r_mcause() == 1 || r_mcause() == 5 || r_mcause() == 7)
    {
        printf("Machine Trap: Cause: PMP Violation at address: 0x%lx\n", r_mtval());
        trapframe->epc += 4; // Skip the instruction and move to next
    }
    else
    {
        // printf("machinetrap(): unexpected mcause 0x%lx\n", r_mcause());
        // printf("            mepc=0x%lx mtval=0x%lx\n", r_mepc(), r_mtval());
        //panic("machinetrap");
        for (;;)
            ;
    }

    machinetrapret();
}

void machinetrapret () {

    uint64 trampoline_machinevec = (uint64)machinevec;
    w_mtvec(trampoline_machinevec);

    trapframe->sm_satp = r_satp();
    trapframe->sm_sp = (uint64)(sm_stack + PGSIZE);
    trapframe->sm_trap = (uint64)machinetrap;
    trapframe->sm_hartid = r_tp();

    // set M Previous Privilege mode to Supervisor.
    unsigned long x = r_mstatus();
    x &= ~MSTATUS_MPP_MASK;
    x |= MSTATUS_MPP_S;
    w_mstatus(x);

    // set S Exception Program Counter to the saved user pc.
    w_mepc(trapframe->epc);

    // tell trampoline.S the user page table to switch to.
    uint64 satp = trapframe->kernel_pagetable; //? MAKE_SATP(trapframe->kernel_pagetable) : 0;

    uint64 trampoline_machineret = (uint64)machineret;
    ((void (*)(uint64))trampoline_machineret)(satp);
}