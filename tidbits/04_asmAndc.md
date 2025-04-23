# ASM and C

## 1 在ASM中调用C

如何在ASM文件中调用C文件中实现的函数？

```asm
.text
.global main
main:
    li a0, 1
    li a1, 2
    j sum
.end
```

```c
#include <stdio.h>
void sum(int a, int b)
{
    printf("%d + %d = %d\n", a, b, a + b);
}
```

在汇编代码中可以直接通过j C语言函数名，来达到调用C语言函数的效果，gcc编译器会编译链接两个文件。

```shell
$ ls
asm.s  c.c
$ gcc c.c asm.s -o asm2c
$ ./asm2c
1 + 2 = 3
```

## 2 在C中调用ASM

如何在C文件中调用ASM文件中实现的函数？

```asm
.text
.global sum
sum:
    add a0, a0, a1
    ret
.end
```

```c
#include <stdio.h>
int sum(int, int);
int main()
{
    printf("%d + %d = %d\n", 1, 2, sum(1, 2));
    return 0;
}
```

gcc编译器会默认将名称为main的函数当作入口点，故而此时C文件中的函数名为main，前面是ASM文件中的函数名为main。

```shell
$ ls
asm.s  c.c
$ gcc c.c asm.s -o c2asm
$ ./c2asm
1 + 2 = 3
```

## 3 在C中嵌入ASM

如何在C文件中使用汇编语言来编写程序？

```c
#include <stdio.h>
int main()
{
    int x = 1, y = 2, z = 0;
    asm volatile(
        "add %0, %1, %2"
        : "=r"(z)
        : "r"(x), "r"(y));
    printf("z = %d\n", z);

    int a = 2, b = 3, c = 0;
    asm volatile(
        "add %[sum1], %[add1], %[add2];"
        "add %[sum2] ,%[add3], %[add4]"
        : [sum1] "=r"(c), [sum2] "=r"(z)
        : [add1] "r"(a), [add2] "r"(b), [add3] "r"(z), [add4] "r"(y));
    printf("c = %d, z = %d\n", c, z);
    return 0;
}

```

主要包含三部分：

* `asm volatile();`里面包裹着汇编指令的信息；
* 汇编指令：双引号包裹汇编指令，寄存器用%[name]来表示；若出现多个汇编指令，则需要加分号；
* 寄存器和C语言变量的对应：两个冒号，分别代表输出操作数列表和输入操作数列表。

注：如果在汇编指令中用%0等数字表示寄存器，则在后面的操作数列表中"=r"和"r"前就不需要指明寄存器名字；否则需要通过[]指明寄存器名字。
