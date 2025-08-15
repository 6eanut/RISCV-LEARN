# Syscall

## 1 思路

之前的所有实验都是在M模式下运行，故而可以随意读写mhartid、mstatus。为了引入U模式，我们需要在mstatus中设置MPP位，以在mret执行后，让机器进入U模式。即，将mstatus的MPP位设为00，表示trap发生前是U-mode，当任务创建完毕，要把第一个任务放在hart上运行时，执行mret，从而进入U模式。

这里需要注意配置PMP，否则进入U模式后，会出现非法访问指令的问题：

```asm
li      t0, 0xffffffff
csrw    pmpaddr0, t0
li      t0, 0xf
csrw    pmpcfg0, t0
```

这样修改完毕之后，要想在任务里面获取mhartid等csr寄存器的值，则会报exception了。

那么想要在U-mode下读csr寄存器，可以借助系统调用来实现。

## 2 实现

在任务里调用系统调用syscall_get_mstatus：

```c
void user_task0(void)
{
    uart_puts("Task0 Created\n");
    reg_t mstatus = syscall_get_mstatus();
    printf("mstatus = %lx\n", mstatus);
    while (1)
    {
        uart_puts("Task0 Running\n");
        task_delay(DELAY);
    }
}
```

syscall_get_mstatus会把该系统调用的系统调用号放到a7寄存器中，并调用ecall触发exception：

```asm
.global syscall_get_mstatus
syscall_get_mstatus:
    li a7, 2
    ecall
    ret
```

触发exception之后，自然就会进入到trap_vector，而后进入trap_handler，当检测到mcause中的cause是8，即U模式环境调用时，会执行do_syscall函数，进行系统调用的分发：

```c
void trap_handler(reg_t mcause)
{
    reg_t cause_code = mcause & MCAUSE_MASK_ECODE;
    if (mcause & MCAUSE_MASK_INTERRUPT)
    {
        ......
    }
    else
    {
        printf("Exception Happened, Exception Code: %ld\n", cause_code);
        switch (cause_code)
        {
        case 8:
            uart_puts("Syscall from U-Mode\n");
            do_syscall();
            break;
        default:
            panic("What can I do");
        }
    }
}
```

do_syscall函数会根据a7寄存器存储的系统调用号进行分发，进而执行真正的函数：

```c
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
```

在getmstatus函数中会获取mstatus的值，并放在a0寄存器中，并且更改mepc的值：

```c
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
```

注意：此时运行在trap上下文中，当trap处理完成后，会从内存中加载原任务的上下文，如果这里只是把mepc和a0的值做改动，等恢复上下文后，值就都被覆盖了，所以需要改动的不是当前上下文中寄存器的值，而是需要改动原任务的上下文中a0和mepc的值。

修改mepc是因为ecall触发的是exception，其会将mepc设为ecall的地址，如果不把mepc改为下一条指令，那么等exception处理完毕，还会重复执行ecall。
