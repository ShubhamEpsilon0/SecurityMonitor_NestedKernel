#include "../security_monitor/syscall.h"


.global sm_print
sm_print:
    li a7, SYS_sm_print
    ecall
    ret


.global sm_log
sm_log:
    li a7, SYS_sm_log
    ecall
    ret