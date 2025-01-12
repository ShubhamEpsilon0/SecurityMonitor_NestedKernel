
#include "kernel/types.h"
#include "kernel/riscv.h"
#include "trapframe.h"
#include "kernel/defs.h"
#include "kernel/param.h"
#include "kernel/memlayout.h"
#include "kernel/spinlock.h"
#include "kernel/syscall.h"
#include "sysproc.h"
#include "snprintf.h"

#include <stdarg.h>

#define MAXLOGS 10240

struct sm_log sysCallLogs[MAXLOGS];
int sysCallLogIndex = 0;

int serializeArg(struct sm_syscall_arg *arg, char* buffer, int bufSize) {
  switch (arg->argType) {
    case INT:
      return snprintf(buffer, bufSize, "%s: %d ", arg->argValue.argName, (int)arg->argValue.argValue);
    case STRING:
      return snprintf(buffer, bufSize, "%s: %s ", arg->argValue.argName, (char *)(arg->argValue.argValue));
    case POINTER:
      return snprintf(buffer, bufSize, "%s: %p ", arg->argValue.argName, (void *)arg->argValue.argValue);
    default:
      return snprintf(buffer, bufSize, "%s: Unknown ", arg->argValue.argName);
  }
}

char * sysCallNumberToSystemCallName (uint32 syscallNumber) {
  switch (syscallNumber) {
    case SYS_exit:
      return "exit";
    case SYS_fork:
      return "fork";
    case SYS_wait:
      return "wait";
    case SYS_pipe:
      return "pipe";
    case SYS_read:
      return "read";
    case SYS_kill:
      return "kill";
    case SYS_exec:
      return "exec";
    case SYS_fstat:
      return "fstat";
    case SYS_chdir:
      return "chdir";
    case SYS_dup:
      return "dup";
    case SYS_getpid:
      return "getpid";
    case SYS_sbrk:
      return "sbrk";
    case SYS_sleep:
      return "sleep";
    case SYS_uptime:
      return "uptime";
    case SYS_open:
      return "open";
    case SYS_write:
      return "write";
    case SYS_mknod:
      return "mknod";
    case SYS_unlink:
      return "unlink";
    case SYS_link:
      return "link";
    case SYS_mkdir:
      return "mkdir";
    case SYS_close:
      return "close";
    default:
      return "Unknown";
  }
}

void sm_log_internal(const char *processName, uint32 syscallNumber, int sysCallArgsCount, struct sm_syscall_arg* syscallArgs)
{
  struct sm_log* log = &sysCallLogs[sysCallLogIndex];
  sysCallLogIndex = (sysCallLogIndex + 1) % MAXLOGS;
  int curLogSize = 0;

  snprintf(log->processName, sizeof(log->processName), "Process: %s ", processName);
  snprintf(log->syscallNumber, sizeof(log->syscallNumber), "SysCall: %s ", sysCallNumberToSystemCallName(syscallNumber));

  memset(log->syscallArgs, 0, sizeof(log->syscallArgs));

  for (int i = 0; i < sysCallArgsCount; i++)
  {
    struct sm_syscall_arg argument = syscallArgs[i]; // Access each argument as an int
    curLogSize += serializeArg(
      &argument,
      log->syscallArgs + curLogSize,
      sizeof(log->syscallArgs) - curLogSize > 40 ? 40 : sizeof(log->syscallArgs) - curLogSize
    );

    if (curLogSize >= sizeof(log->syscallArgs) - 1) {
      break;
    }
  }

  //printf("[Protected Log: %s, %s, Arguments: { %s }]\n", log->processName, log->syscallNumber, log->syscallArgs);
}

/*================================================ Sys Calls=========================================================== */

uint64 sm_log(void){
    uint64 processName;
    uint32 syscallNumber;
    uint32 sysArgsCount;
    uint64 sysArgs;

    argaddr(0, &processName);
    argint(1, (int *)&syscallNumber);
    argint(2, (int *)&sysArgsCount);
    argint(3, (int *)&sysArgs);

    sm_log_internal((char *)processName, syscallNumber, sysArgsCount, (struct sm_syscall_arg *)sysArgs);

    return 0;
}

uint64 sm_print(void)
{
  printf("Protected Logs%d\n\n\n", sysCallLogIndex);

  for (int i = 0; i < sysCallLogIndex; i++) {
    printf("[Protected Log: %s, %s, Arguments: { %s }]\n", sysCallLogs[i].processName, sysCallLogs[i].syscallNumber, sysCallLogs[i].syscallArgs);
  }
  
  return 0;
}
