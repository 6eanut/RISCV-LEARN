#include "../include/os.h"

void getmhartid()
{
    asm volatile(
        "csrr a0, mhartid\n"
        "csrr t0, mscratch\n"
        "csrr t1, mepc\n"
        "addi t1, t1, 4\n"
        "sw t1, 31*4(t0)\n"
        "sw a0, 9*4(t0)\n"
        :
        :);
}

void getmstatus()
{
    asm volatile(
        "csrr a0, mstatus\n"
        "csrr t0, mscratch\n"
        "csrr t1, mepc\n"
        "addi t1, t1, 4\n"
        "sw t1, 31*4(t0)\n"
        "sw a0, 9*4(t0)\n"
        :
        :);
}

void do_syscall()
{
    syscallID syscall_id = getSyscallID();
    printf("syscallID = %lx\n", syscall_id);
    switch (syscall_id)
    {
    case 1:
        getmhartid();
        break;
    case 2:
        getmstatus();
        break;
    default:
        panic("Unknown SyscallID\n");
    }
}
