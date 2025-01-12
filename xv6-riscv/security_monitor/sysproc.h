#include "kernel/types.h"

struct sm_arg_value {
    const char argName[30];
    uint64 argValue;
};

struct sm_log {
    char processName[35];
    char syscallNumber[35];
    char syscallArgs[130];
};

enum sm_syscall_arg_type {
    INT,
    STRING,
    POINTER
};

struct sm_syscall_arg
{
    enum sm_syscall_arg_type argType;
    struct sm_arg_value argValue;
};

extern struct sm_log sysCallLogs[];