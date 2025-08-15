# MultiTasks

## 1 思路

在上下文转换一节实现了协作式多任务，即任务会主动放弃对hart的使用；在硬件定时器一节实现了定时器中断的识别和处理。本节在硬件定时器中断处理程序中加入任务切换，以此实现抢占式多任务。

当一个任务在执行过程中发生trap时，在trap_vector里面会先保存上下文，而后去做trap处理函数；当trap处理函数执行完毕后，会更新mscratch的值，并且通过mret返回到mepc所指向的代码位置。

为了实现抢占式多任务，即需要在trap处理函数里面添加部分内容，这部分内容会把mscratch的值更新为下一个要执行任务的上下文内存起始地址。

由此可见，每个任务的上下文不仅包含通用寄存器，还需要保存mepc这个csr寄存器，这在任务创建时可以初始化成ra，因为mepc在非trap情况下不会使用。

## 2 mscratch

要在定时器中断处理过程中，将mscratch改为下一个执行任务的上下文内存地址。

```asm
.global switch_to
switch_to:
    csrw mscratch, a0
    ret
```

a0即为下一个执行任务的上下文内存地址，将其赋给mscratch；在上下文中多保存了mepc。

## 3 兼容协作式多任务

有了抢占式多任务，但也想保留协作式多任务的机制，可以采用软中断的方式来完成。

trap里面的interrupt分为local和global，global由PLIC管理，local由CLINT管理，local又分为software和timer两种interrupt。

如果将协作式多任务机制也通过interrupt来实现，那么就可以兼容原来的switch_to。

```c
// 需要对mie的msie和mtie都使能
void clint_init()
{
    // init mtimecmp reg
    update_mtimecmp();
  
    // set mie.sie and mie.tie
    enable_mie_mtie();
    enable_mie_msie();
}

// 在任务中调用task_yield函数，改写CLINT中的MSIP寄存器，从而触发软件中断
void task_yield()
{
    *(ptr_t *)(CLINT_BASE + CLINT_MSIP_BASE + 4 * read_mhartid()) = 1;
}

// 在中断处理函数中进行软件中断处理，先关闭CLINT中的MSIP位，而后执行上下文切换
void software_interrupt_handler()
{
    *(ptr_t *)(CLINT_BASE + CLINT_MSIP_BASE + 4 * read_mhartid()) = 0;
    switch_task();
}
```

CLINT中有MSIP寄存器(每个hart独有一个)，其32位，但只有最低位有效，且始终映射hart中的mip.msip。软件中断的触发和处理主要就是修改CLINT中的MSIP，这里我们把协作式多任务融入软件中断，把抢占式多任务融入时钟中断。除了中断触发条件和结束中断的处理不同外，与任务切换部分的过程一样。
