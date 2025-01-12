#include "protected_log.h"

#include "security_monitor/sysproc.h"
#include "sm_calls.h"

void protected_log (char* processName, uint32 syscallNumber, uint32 sysCallArgsCount, ...) {
    struct sm_syscall_arg* syscallArgs= kalloc();
    va_list args;
    va_start(args, sysCallArgsCount);
    for (int i = 0; i < sysCallArgsCount; i++) {
        struct sm_syscall_arg arg = va_arg(args, struct sm_syscall_arg);
        memmove(syscallArgs + i, &arg, sizeof(struct sm_syscall_arg));
    }
    va_end(args);
    sm_log(processName, syscallNumber, sysCallArgsCount, syscallArgs);
    kfree((char*)syscallArgs);
}