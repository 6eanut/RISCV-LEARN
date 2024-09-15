# .c->.s->.o->.out

## 0. C语言程序->可运行的机器语言程序

从C语言程序到可运行的机器语言程序需要四步，即编译器将.c文件编译为.s文件，汇编器将.s文件汇编为.o文件，连接器将同为机器语言模块的目标文件和库文件链接为.out文件，最后加载器运行机器语言程序,即可执行文件。

![1726380439755](image/03_c-s-o-out/c-s-o-out.png)

## 1. 保存寄存器/临时寄存器

在函数调用前后值不会发生改变的是保存寄存器，如s、sp等；值可能改变的叫临时寄存器，包括t、a、ra等

## 2. 汇编器

### 2.1 伪指令

汇编器不仅支持处理器可以理解的汇编指令，还支持方便汇编语言程序员和编译器开发者的伪指令

主要分为[依赖于零寄存器x0](image/03_c-s-o-out/pseudoinstructions-x0yes.png)和[与零寄存器x0无关](image/03_c-s-o-out/pseudoinstructions-x0no.png)的伪指令两类，它们分别是32条和28条

### 2.2 汇编器指示符

[assembler directives](image/03_c-s-o-out/assembler-directives.png)

以英文句号开头的命令

通知汇编器在何处放置代码(.text)和数据(.data)、指定程序中使用的代码和数据常量等

### 2.3 ELF

**E**xectuable and **L**inkable **F**ormat

汇编器将.s文件汇编生成ELF(可执行可链接格式)标准格式目标.o文件，例如[插入排序](image/03_c-s-o-out/insert_sort.png)
