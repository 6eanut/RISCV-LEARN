# Software Timer

## 1 思路

前面的硬件定时器是全局唯一的，即所有hart共用一个CLINT中的mtime寄存器。要想实现多个定时器，那就需要在软件层面基于mtime来实现。

本实验的思路是，创建软件定时器数组，数组元素包含定时器触发条件、触发条件为真时要执行的函数以及函数的参数，如下：

```c
struct swtimer
{
    void (*func)(void *);
    void *arg;
    uint32_t timeout;
};

struct swtimer swtimer_list[MAX_SWTIMER_NUMS];
```

然后在某个任务中，创建多个软件定时器，并且在每次定时器中断发生时，判断软件定时器数组中是否有触发条件为真的情况，若有则调用要执行的函数，执行完毕后删除该软件定时器。

本实验比较简单，可能只是展示定时器的一种用法。
