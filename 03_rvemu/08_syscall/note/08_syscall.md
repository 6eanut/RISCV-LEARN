# Syscall

## 1 思路

经过前面的调试，目前已经验证了加减乘除的整数运算是没问题的，包括循环控制流。但是想要实现printf等功能还需要完善系统调用的模拟执行。

RISC-V规定当执行ecall时，会把系统调用号放在a7寄存器中，a0到a6是参数，所以当遇到ecall指令后，可以读取a7的系统调用号id，然后去做相应的操作，具体就是借用X86的系统调用来模拟执行。

## 2 实现

```c
int main(int argc, char *argv[])
{
    // ...
    while (true)
    {
        machine_step(&machine);

        uint64_t syscall_id = machine_get_gp_reg(&machine, a7);
        uint64_t ret = do_syscall(&machine, syscall_id);
        machine_set_gp_reg(&machine, a0, ret);
    }

    return 0;
}
```

系统调用执行前，需要读a7的值，从而决定执行什么样的系统调用，系统调用执行完之后会把返回结果写回a0寄存器，即是否成功执行。

```c
uint64_t do_syscall(machine_t *machine, uint64_t syscall_id)
{
    uint64_t ret = 0;
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
```

这里只实现了printf所用到的一些系统调用，以sys_brk为例：

```c
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
```

sys_brk是Linux中管理进程堆内存的一个系统调用，malloc和free会用到这个系统调用。

* 输入参数：新的堆空间结束地址；
* 输出参数：返回该地址；
* 操作：堆空间扩展或缩小。

目前模拟器已经算是基本完成了，但是还有许多指令有待验证，后续会慢慢debug。

## 3 Debug

这里记录一些碎的东西：

* indirect branch和direct branch的区别在于，后者能够在编译之后、运行之前知道目标branch地址，而前者不可以，因为前者的目标地址的计算依赖于寄存器的具体值，而这在运行时才能知道，后者的目标地址的计算会硬编码在指令里面或者是相对于当前PC的偏移值；
* 有一种加速模拟的方式：统计某个代码块的执行次数，如果足够多，那么将RISC-V指令翻译成C语言代码，而后经过clang -O3编译成X86的指令，存储在内存中，之后再要执行这块代码，就可以直接执行原生指令，而不需要再decode和interpret；
