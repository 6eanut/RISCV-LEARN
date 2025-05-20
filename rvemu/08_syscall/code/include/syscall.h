#ifndef SYSCALL_H
#define SYSCALL_H
// https://github.com/riscv-software-src/riscv-pk/blob/master/pk/syscall.h

#define SYS_exit 93
#define SYS_fstat 80
#define SYS_brk 214
#define SYS_close 57
#define SYS_write 64

uint64_t do_syscall(machine_t *, uint64_t);

#endif