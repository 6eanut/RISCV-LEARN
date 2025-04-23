# Context Switch

## 1 思路

因为只考虑单个hart的情况，所以startup实验首先在汇编代码中判断当前hart id是否0，否则wfi，而后跳转入C语言函数；为了方便调试，uart实验通过uart和qemu模拟串口线实现了打印字符串的功能；ld在链接目标文件时会决定os.elf中的各个段会如何放置在物理内存上，为了自己管理内存，memory实验通过linker script来决定段放在物理内存的哪个位置，以及内存中数据段、代码段的划分和堆空间的划分和分配。至此，一些准备工作基本完成，操作系统比较贴近用户的功能还是执行任务。

因为这里只有单个hart，故而如果要执行多任务，那么一定是并发的，即涉及任务的调度，这里先实现协作式多任务，即每个任务会主动放弃hart的使用权，供其他任务使用。多任务指的是存在多个互不相关的任务，这和函数调用有区别，在一个任务中，无论内部存在怎样的函数调用过程，都被认为是一个任务。而多任务指的是，多个任务，先执行任务1到a位置，再去执行任务2，然后任务2主动放弃hart，拐回来接着任务1的a位置继续执行，给人一种多个任务在同时执行的感觉。

大致思路如下：

* 每个任务都有自己的上下文(32个寄存器)和自己的栈空间(用于任务内部的函数调用)，三者一一对应；
* mscratch寄存器用于指定当前hart正在运行的任务的上下文在内存中的地址，用于标识当前正在执行的任务是哪个；
* 内存管理初始化后，需要初始化mscratch寄存器的值为0；
* 而后进入操作系统，创建要执行的多个任务。创建任务的过程就是为任务分配上下文和栈空间的过程，并将任务的入口赋值给上下文的ra寄存器，将任务的栈空间地址赋值给上下文的sp寄存器；
* 当任务创建完毕后，开始执行调度。有一个全局变量_current来表示当前正在hart上执行的任务标号，调度的过程就是先更新_current指向下一个标号的任务，然后保存上下文、恢复上下文并执行下一个任务：
  * 保存上下文即将当前寄存器的值存储到mscratch指向的内存上下文中，mscratch指向的是正在hart上运行的任务的上下文在内存中的旧值，将当前寄存器的值写入该旧值中，即保存当前运行状态；
  * 恢复上下文即将要调度的任务的上下文写入寄存器中，以供hart执行；
  * 执行即，因为恢复上下文过程更改了寄存器ra的值为将要执行任务的地址，故而ret即可执行ra所指的代码。
* 当任务创建完毕后，第一次执行调度，不会存在保存寄存器的操作；但是后面任务内部主动的放弃hart，则需要保存寄存器。

## 2 实现

### 2-1 任务的创建

```c
uint8_t __attribute__((aligned(16))) tasks_stack[MAX_TASKS][STACK_SIZE];
struct context tasks_ctx[MAX_TASKS];

int task_create(void (*task_entry)(void))
{
    if (_top < MAX_TASKS)
    {
        tasks_ctx[_top].ra = (reg_t)task_entry;
        tasks_ctx[_top].sp = (reg_t)&tasks_stack[_top][STACK_SIZE];
        _top++;
        return 0;
    }
    else
        return -1;
}
```

一些说明：

* 这里提前把所有上下文空间和栈空间都分配好了，没有采用动态分配；
* 将上下文的ra改为任务入口，上下文的sp改为相应的栈空间，_top标记了目前空的位置。

### 2-2 任务的调度

```c
void schedule()
{
    if (!_top)
        panic("No Task has been created");
    else
    {
        _current = (_current + 1) % _top;
        struct context *next = &tasks_ctx[_current];
        switch_to(next);
    }
}

.text
.global switch_to
.balign 4
switch_to:
    csrrw t6, mscratch, t6
    beqz t6, init

    write_ctx_from_reg t6
    csrr t5, mscratch
    STORE t5, 30*REG_SIZE(t6)

init:
    csrw mscratch, a0
    mv t6, a0
    write_reg_from_ctx t6
    ret
.end
```

一些说明：

* 调度会找到下一个任务，并将下一个任务在内存中的上下文作为参数，传递给switch_to，以供恢复上下文；
* switch_to就是保存上下文write_ctx_from_reg，恢复上下文write_reg_from_ctx，执行下一个任务即ret；
* 上下文就是32个寄存器，因为tp和gp是hart全局的，故这里不保存。
