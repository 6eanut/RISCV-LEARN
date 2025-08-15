# Lock

## 1 思路

多任务场景下，不得不提到临界区的问题。当存在多个任务会对一个共享变量进行读写操作时，这涉及到数据同步的问题。因为存在定时器中断导致任务切换，若一个任务只在共享变量值为0时，将其加1，当其判断值为0为真且在加1操作执行前，发生了定时器中断，导致切换到另一个任务，而另一个任务会对该共享变量加1，此时再回到原任务，又会执行一遍加1，导致最终结果是2，与预期不符，出错。

像这样，对共享变量的读写操作的代码叫做Critical Section，要想避免上述情况出现，需要用Lock来锁住该临界区，以避免数据同步出问题。

锁的实现，往往只能借助由硬件提供的原子操作，在软件层面，则体现在利用原子操作汇编指令，在RISC-V中指的是RVA。

## 2 RV32A

实现锁主要用到amoswap.w.aq和amoswap.w.rl指令，即Atomic Memory Operation SWAP . Word AQuire和ReLease。

```asm
// 锁的获取
spin_lock_asm:
    li t0, 1
    amoswap.w.aq a0, t0, (a0)
    ret

// 锁的释放
spin_unlock_asm:
    amoswap.w.rl x0, x0, (a0)
    ret
```

如果是单纯的原子交换操作，可以使用amoswap.w指令，但如果是给临界区加锁，必须使用aq和rl，否则可能会出现CPU或编译器对指令进行重排序，导致临界区内的指令顺序被调度到其他位置，从而影响结果。

上述写法其实并没有实现真正意义上的自旋，为了方便调试，这里直接返回了锁原本的内容，并没有自旋swap操作，这是为了方便调试：

```c
void user_task2(void)
{
    uart_puts("Task2_With_Lock -- Created\n");
    while (1)
    {
        uart_puts("Task2_With_Lock -- Enter \n");
        while (spin_lock(&cs_lock))
        {
            uart_puts("Another Task Has The Lock, So Wait!\n");
            task_delay(DELAY);
        }
        uart_puts("Task2_With_Lock -- Enter The Critical Section, Get The Lock\n");
        for (int i = 0; i < 10; i++)
        {
            printf("Task2_With_Lock -- Operation %d Finished\n", i);
            task_delay(DELAY);
        }
        spin_unlock(&cs_lock);
        uart_puts("Task2_With_Lock -- Exit The Critical Section, Release The Lock\n");
    }
}
```

真正的自旋效果的锁如下：

```asm
spin_lock_asm:
    li t0, 1
try_agin:
    amoswap.w.aq t1, t0, (a0)
    bnez t1, try_again
    ret
```

aq和rl必须一一对应进行配对，以保证可见性。

> 可见性保证：如果任务A用 `.rl` 释放锁，任务B用 `.aq` 获取锁，则，
>
> * 任务A在 `.rl` 之前 的所有操作一定对任务B在 `.aq` 之后的操作可见；
> * 这种可见性是通过 禁止编译器和CPU的重排序 实现的。
