# MultiTasks

## 1 思路

在上下文转换一节实现了协作式多任务，即任务会主动放弃对hart的使用；在硬件定时器一节实现了定时器中断的识别和处理。本节在硬件定时器中断处理程序中加入任务切换，以此实现抢占式多任务。

当一个任务在执行过程中发生trap时，在trap_vector里面会先保存上下文，而后去做trap处理函数；当trap处理函数执行完毕后，会更新mscratch的值，并且通过mret返回到mepc(trap处理函数的返回值)所指向的代码位置。

为了实现抢占式多任务，即需要在trap处理函数里面添加部分内容，这部分内容会把mscratch的值更新为下一个要执行任务的上下文内存起始地址，会将下一个要执行任务的mepc作为返回值传递给trap_vector。

由此可见，每个人物的上下文不仅包含通用寄存器，还需要保存mepc这个csr寄存器，这在任务创建时可以初始化成ra，因为mepc在非trap情况下不会使用。

## 2 mscratch和mepc

要在定时器中断处理过程中，将mscratch改为下一个执行任务的上下文内存地址，并且把下一个执行任务的mepc作为参数返回。

```asm
.global preemptive_switch
preemptive_switch:
    csrw mscratch, a0

    lw a0, 31*REG_SIZE(a0)
    ret
```

a0即为下一个执行任务的上下文内存地址，将其赋给mscratch；在上下文中多保存了mepc，故而从上下文中取出mepc，作为返回值。

## 3 兼容协作式多任务

有了抢占式多任务，但也想保留协作式多任务的机制，这里从逻辑上把任务的切换分为了三类：1系统执行第一个任务；2协作式多任务；3抢占式多任务。

```asm
.global switch_from_null_to_first
.balign 4
switch_from_null_to_first:
    csrrw t6, mscratch, t6
    csrw mscratch, a0
    mv t6, a0
    write_reg_from_ctx t6
    ret

.global preemptive_switch
preemptive_switch:
    csrw mscratch, a0

    lw a0, 31*REG_SIZE(a0)
    ret

.global collaborative_switch
collaborative_switch:
    csrrw t6, mscratch, t6
    csrw mepc, ra
    write_ctx_from_reg t6

    csrw mscratch, a0

    mv t6, a0
    write_reg_from_ctx t6
    ret
```

* 系统执行第一个任务只需要把mscratch改为第一个任务上下文，并把上下文读到寄存器中；
* 协作式多任务需要保存原寄存器内容，而后切换到下一任务上下文，并将上下文读到寄存器中，因为协作式多任务是主动放弃hart，不存在中断，故而需要手动将mepc更新为ra，以供抢占式多任务读取；
* 抢占式多任务通2。
