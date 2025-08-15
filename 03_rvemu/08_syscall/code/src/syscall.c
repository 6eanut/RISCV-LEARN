#include "../include/rvemu.h"

uint64_t sys_exit(machine_t *machine)
{
    uint64_t code = machine_get_gp_reg(machine, a0);
    exit(code);
}

uint64_t sys_fstat(machine_t *machine)
{
    uint64_t fd = machine_get_gp_reg(machine, a0);
    uint64_t addr = machine_get_gp_reg(machine, a1);
    return fstat(fd, (struct stat *)TO_HOST(addr));
}

uint64_t sys_brk(machine_t *machine)
{
    uint64_t addr = machine_get_gp_reg(machine, a0);
    if (addr == 0)
        addr = machine->mmu.guest_alloc;
    assert(addr > TO_GUEST(machine->mmu.host_base));
    uint64_t size = addr - machine->mmu.guest_alloc;
    mmu_alloc(&machine->mmu, size);
    return addr;
}

uint64_t sys_close(machine_t *machine)
{
    uint64_t fd = machine_get_gp_reg(machine, a0);
    if (fd > 2)
        return close(fd);
    return 0;
}

uint64_t sys_write(machine_t *machine)
{
    uint64_t fd = machine_get_gp_reg(machine, a0);
    uint64_t ptr = machine_get_gp_reg(machine, a1);
    uint64_t len = machine_get_gp_reg(machine, a2);
    return write(fd, (void *)TO_HOST(ptr), (size_t)len);
}

uint64_t do_syscall(machine_t *machine, uint64_t syscall_id)
{
    uint64_t ret = 0;
#ifdef DEBUG
    debug_syscall(machine);
#endif
    switch (syscall_id)
    {
    case SYS_exit:
        ret = sys_exit(machine);
        break;
    case SYS_fstat:
        ret = sys_fstat(machine);
        break;
    case SYS_brk:
        ret = sys_brk(machine);
        break;
    case SYS_close:
        ret = sys_close(machine);
        break;
    case SYS_write:
        ret = sys_write(machine);
        break;
    default:
        MYEXIT("syscall unimplement");
    }
    return ret;
}