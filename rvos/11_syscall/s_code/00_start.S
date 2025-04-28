#include "../c_code/include/platform.h"

    .equ    STACK_SIZE, 1024
    .global _start
    .text
_start:
    csrr t0, mhartid
    bnez t0, park
    la sp, stacks+STACK_SIZE

    li      t0, 0xffffffff
	csrw    pmpaddr0, t0
	li      t0, 0xf
	csrw    pmpcfg0, t0

    li t0, 1<<7
    csrs mstatus, t0

    j start_kernel

park:
    wfi
    j park

.balign 16
stacks:
    .skip STACK_SIZE * MAXNUM_CPU
    .end