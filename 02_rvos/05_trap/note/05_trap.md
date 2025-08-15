# Trap

## 1 思路

前面实现了协作式多任务，但是现在的操作系统往往采用的是抢占式多任务，这意味着操作系统会剥夺某个任务对hart的使用权，这涉及到interrupt。在RISC-V中外部的interrupt和内部的exception总称为trap，两者在处理逻辑上区别不大，下面是大致过程。

* trap初始化：
  * exception：当机器执行到非法指令时，会发生exception，系统会给程序一次“改过自新”的机会，即去运行一段exception处理函数，那么这个处理函数的地址在哪？由mtvec寄存器指定，故trap的初始化是在操作系统启动时给mtvec赋值，并定义trap_vector函数。执行完成后，会再次执行发生exception的指令，即mepc；
  * interrupt：当外部中断来临时，机器会执行完当前指令，而后去执行trap处理函数，这个函数的地址同样由mtvec寄存器指定，处理函数执行完毕，会返回到原控制流执行的下一条指令，即mepc；
  * 那么二者都会去走向mtvec寄存器指向的处理函数，如何区别到底是什么原因导致的trap？这需要用到mcause和mtval。
* trap的top half：
  * 该部分由hart自动执行，属于硬件层面；即当程序执行了非法指令或发生外部中断时，hart会这么做；
  * mstatus寄存器记录了xIE中断使能位(用于trap处理过程中屏蔽trap)，xPIE之前的中断使能位(用于恢复)，xPP保存trap发生前的权限级别；这些hart会自动更新；
  * 当trap发生时，pc会被设置为mtvec，mepc用于记录原控制流执行到哪了，以便trap处理完成后回来继续执行，mepc会被设置为原pc或原pc+1；
  * 更新mcause和mtval的值，以指导mtvec处函数的执行，并将机器改为M模式；
  * 注：mtvec处的trap_vector函数有两种模式，由mtvec寄存器的低2位决定是direct还是vectored模式，前者意思是mtvec处的函数是一个，后者的意思是metvec处的地址是多个函数；
* trap的bottom half：
  * 软件层面，开始执行mtvec寄存器指向的trap_vector函数；由于mtvec的编码格式规范，要求trap_vector的地址必须是4B对齐；
  * trap_vector保存原控制流上下文；
  * trap_vector调用C语言的trap_handler函数；
  * trap_handler根据mcause和mtval来做处理，最后返回trap_vector；
  * trap_vector恢复原控制流上下文；
* trap的返回：
  * 执行mret指令，将pc改为mepc。

在实验四的基础上，通过在任务里面查看mtvec寄存器的值，会发现为0，若添加*(int *)0x00000000 = 1;指令则会发生exception，系统会执行玩初始化和top half部分，但是当要跳转到由mtvec指向的地址时会出错，因为这里没有给mtvec赋值，即初始化部分为空，只执行了top half，故而在bottom half阶段会出错。

所以接下来就是实现初始化和bottom half，即定义trap_vector函数，并赋值给mtvec。

## 2 实现

### 2-1 初始化

```c
void trap_init()
{
    asm volatile("csrw mtvec, %[trap_vector]" : : [trap_vector] "r"(trap_vector));
}
```

其实就是把tarp_vector的地址写给mtvec寄存器。

### 2-2 trap_vector

```asm
.balign 4
trap_vector:
    csrrw t6, mscratch, t6
    write_ctx_from_reg t6
    csrr t5, mscratch
    STORE t5, 30*REG_SIZE(t6)

    csrw mscratch, t6

    csrr a0, mepc
    csrr a1, mcause
    call trap_handler

    csrw mepc, a0

    csrr t6, mscratch
    write_reg_from_ctx t6

    mret
```

因为mtvec寄存器中的BASE字段是[31:2]，故trap_vector函数的地址必须是4B对齐；mtvec寄存器的后两位记录了该trap_vector函数是Direct还是Vectored的mode，前者表示所有trap的处理都会将pc设置为BASE，后者表示发生interrupt时pc被设置为BASE+mcause*4。

.h文件既支持C语言中的#define宏定义，还支持汇编语言中的.macro.end宏定义，故而这里把上下文的保存和恢复放在了.h文件中，但是这个头文件只能被汇编文件include，不能被C语言文件include，否则编译器会报错。另外，s和S文件都支持.include，只有S支持#include。

trap_handler有两个参数，第一个是mepc，第二个是mcause；保存上下文即保存发生trap的任务的上下文，恢复上下文也是指恢复发生trap的任务的上下文，即同一个任务，故mscratch在trap_handler前后指向的是同一个内存地址中的上下文。

### 2-3 trap_handler

```c
reg_t trap_handler(reg_t mepc, reg_t mcause)
{
    reg_t return_pc = mepc;
    reg_t cause_code = mcause & MCAUSE_MASK_ECODE;
    if (mcause & MCAUSE_MASK_INTERRUPT)
    {
        printf("Interruption Happened, Exception Code : %ld\n", cause_code);
        switch (cause_code)
        {
        case 3:
            uart_puts("Software Interruption\n");
            break;
        case 7:
            uart_puts("Timer Interruption\n");
            break;
        case 11:
            uart_puts("External Interruption\n");
            break;
        default:
            uart_puts("Unkonwn Interruption\n");
            break;
        }
    }
    else
    {
        printf("Exception Happened, Exception Code: %ld\n", cause_code);
        panic("What can I do");
    }
    return return_pc;
}
```

一些说明：

* 首先要判断是interrupt还是exception，这个通过mcause的第31位来判断；
* 而后根据mcause的[30:0]位来细致判断到底是什么原因，打印出相应的信息；
* 注意，mepc的设置是hart自动完成的，即interrupt会mepc=pc+1，exception会mpec=pc，故而不需要在软件层面根据trap类型来设置mepc。
