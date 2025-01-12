
#include "kernel/types.h"
#include "kernel/param.h"
#include "trapframe.h"
#include "syscall.h"
#include "kernel/defs.h"

static uint64
argraw(int n)
{
  switch (n) {
  case 0:
    return trapframe->a0;
  case 1:
    return trapframe->a1;
  case 2:
    return trapframe->a2;
  case 3:
    return trapframe->a3;
  case 4:
    return trapframe->a4;
  case 5:
    return trapframe->a5;
  }
  panic("argraw");
  return -1;
}

// Fetch the nth 32-bit system call argument.
void
argint(int n, int *ip)
{
  *ip = argraw(n);
}

// Retrieve an argument as a pointer.
// Doesn't check for legality, since
// copyin/copyout will do that.
void
argaddr(int n, uint64 *ip)
{
  *ip = argraw(n);
}

// // Prototypes for the functions that handle system calls.
extern uint64 sm_print(void);
extern uint64 sm_log(void);

// An array mapping syscall numbers from syscall.h
// to the function that handles the system call.
static uint64 (*syscalls[])(void) = {
    [SYS_sm_print] = sm_print,
    [SYS_sm_log] = sm_log,
};

void
syscall(void)
{
  int num;
  num = trapframe->a7;
  if(num > 0 && num < NELEM(syscalls) && syscalls[num]) {
    // Use num to lookup the system call function for num, call it,
    // and store its return value in p->trapframe->a0
    trapframe->a0 = syscalls[num]();
  } else {
    printf("unknown sys call %d\n", num);
    trapframe->a0 = -1;
  }
}
