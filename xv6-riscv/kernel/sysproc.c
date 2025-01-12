#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "syscall.h"
#include "protected_log.h"
#include "sm_calls.h"

#include "security_monitor/sysproc.h"

extern uint64 etext;

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  protected_log(myproc()->name, SYS_exit, 1, (struct sm_syscall_arg){INT, {"Exit Code", n}});
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  protected_log(myproc()->name, SYS_getpid, 0);
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  protected_log(myproc()->name, SYS_fork, 0);
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  protected_log(myproc()->name, SYS_wait, 1, (struct sm_syscall_arg){POINTER, {"Address", p}});
  return wait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int n;

  argint(0, &n);
  addr = myproc()->sz;
  protected_log(myproc()->name, SYS_sbrk, 1, (struct sm_syscall_arg){POINTER, {"Size", n}});
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  protected_log(myproc()->name, SYS_sleep, 1, (struct sm_syscall_arg){INT, {"Time", n}});
  if(n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  protected_log(myproc()->name, SYS_kill, 1, (struct sm_syscall_arg){INT, {"PID", pid}});
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;
  protected_log(myproc()->name, SYS_uptime, 0);
  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

uint64 sys_protected_logs (void)
{
  sm_print();
  return 0;
}

uint64 sys_testKernelCodeIntegrity (void)
{
  uint64 kernelStart = KERNBASE;
  uint64 kernelCodeEnd = (uint64)&etext;

  printf("Kernel End: %p\n", (void *)kernelCodeEnd);

  for (char *i = (char *)kernelStart; i < (char *)kernelCodeEnd; i += 1)
  {
    // Trying to write Kernel Code.
    //*i = 'A';
    volatile char x = *i;
    (void)x;
  }
  return 0;
}
