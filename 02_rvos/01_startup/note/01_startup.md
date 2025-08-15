# Startup

## 1 思路

在x86架构上编写操作系统内核源码，经过交叉编译工具链编译成riscv32的elf文件，通过qemu模拟硬件运行该elf文件。

## 2 环境

```shell
# 安装交叉编译工具链、调试工具以及qemu：
$ sudo apt install build-essential gcc make perl dkms git gcc-riscv64-unknown-elf gdb-multiarch qemu-system-misc
# 环境
$ neofetch
            .-/+oossssoo+/-.               jiakai@jiakai-VMware-Virtual-Platfor 
        `:+ssssssssssssssssss+:`           ------------------------------------ 
      -+ssssssssssssssssssyyssss+-         OS: Ubuntu 24.04.2 LTS x86_64 
    .ossssssssssssssssssdMMMNysssso.       Host: VMware Virtual Platform None 
   /ssssssssssshdmmNNmmyNMMMMhssssss/      Kernel: 6.11.0-21-generic 
  +ssssssssshmydMMMMMMMNddddyssssssss+     Uptime: 18 mins 
 /sssssssshNMMMyhhyyyyhmNMMMNhssssssss/    Packages: 1664 (dpkg), 10 (snap) 
.ssssssssdMMMNhsssssssssshNMMMdssssssss.   Shell: bash 5.2.21 
+sssshhhyNMMNyssssssssssssyNMMMysssssss+   Resolution: 1280x800 
ossyNMMMNyMMhsssssssssssssshmmmhssssssso   DE: GNOME 46.0 
ossyNMMMNyMMhsssssssssssssshmmmhssssssso   WM: Mutter 
+sssshhhyNMMNyssssssssssssyNMMMysssssss+   WM Theme: Adwaita 
.ssssssssdMMMNhsssssssssshNMMMdssssssss.   Theme: Yaru [GTK2/3] 
 /sssssssshNMMMyhhyyyyhdNMMMNhssssssss/    Icons: Yaru [GTK2/3] 
  +sssssssssdmydMMMMMMMMddddyssssssss+     Terminal: gnome-terminal 
   /ssssssssssshdmNNNNmyNMMMMhssssss/      CPU: Intel i5-10210U (6) @ 2.112GHz 
    .ossssssssssssssssssdMMMNysssso.       GPU: 00:0f.0 VMware SVGA II Adapter 
      -+sssssssssssssssssyyyssss+-         Memory: 1597MiB / 13282MiB 
        `:+ssssssssssssssssss+:`
            .-/+oossssoo+/-.   
```

## 3 Makefile

```makefile
# 交叉编译工具链以及编译选项和链接选项
CC = riscv64-unknown-elf-gcc 
CFLAGS = -nostdlib -fno-builtin -g -Wall -march=rv32g -mabi=ilp32
LDFLAGS = -Ttext=0x80000000 

# 硬件模拟及运行选项
QEMU = qemu-system-riscv32
QFLAGS = -nographic -smp 1 -machine virt -bios none

# 调试工具
GDB = gdb-multiarch
OBJDUMP = riscv64-unknown-elf-objdump

# 源文件及目标文件路径
OUT_PATH = out
C_PATH = c_code/src
S_PATH = s_code

# 目标文件
OBJS_C := $(patsubst $(C_PATH)/%.c, $(OUT_PATH)/%.o, $(wildcard $(C_PATH)/*.c))
OBJS_S := $(patsubst $(S_PATH)/%.S, $(OUT_PATH)/%.o, $(wildcard $(S_PATH)/*.S))
OBJS = $(OBJS_S) $(OBJS_C)  
ELF = out/os.elf

# 默认构建目标
.DEFAULT_GOAL := all

# 依赖关系
all : ${OUT_PATH} ${ELF}

${OUT_PATH}:
	mkdir -p $@

${ELF}:${OBJS}
	${CC} ${CFLAGS} ${LDFLAGS} -o ${ELF} ${OBJS}

${OUT_PATH}/%.o:${S_PATH}/%.S
	${CC} ${CFLAGS} -c -o $@ $<

${OUT_PATH}/%.o:${C_PATH}/%.c
	${CC} ${CFLAGS} -c -o $@ $<

# 规则
run : all
	${QEMU} ${QFLAGS} -kernel ${ELF}

debug : all
	${QEMU} ${QFLAGS} -kernel ${ELF} -s -S &
	${GDB} ${ELF} -q -x ./gdbinit

code : all
	${OBJDUMP} -S ${ELF} 

clean:
	rm -rf ${OUT_PATH} ${ELF}
```

关于Makefile的一些解释：

* CFLAGS：
  * -nostdlib：不链接标准C库，适用于逻辑环境；
  * -fno-builtin：禁用内置函数优化；
  * -g：生成调试信息；
  * -Wall：启用所有警告信息；
  * -march=rv32g：指定目标架构为RV32G；
  * -mabi=ilp32：指定abi为int、long、pointer都是32位；
* LDFLAGS：
  * -Ttext=0x80000000：设置代码段的起始地址为0x8000000；
* QFLAGS：
  * -nographic：不适用图形界面；
  * -smp 1：单核处理器；
  * -machine virt：使用virt虚拟机器；
  * -bios none：不加载bios，直接从内核启动；
* run：通过-kernel os.elf来指定内核；
* debug：
  * -s：qemu在1234端口启动gdb服务器；
  * -S：启动时暂停CPU；
  * -q：gdb静默模式；
  * -x gdbinit：使用gdb脚本初始化gdb；
* code：反汇编生成带源码的汇编代码。

qemu的virt虚拟机器，会先从0x1000地址进行bootloader，而后将pc设为0x80000000，即内核代码位置。故而-bios none可以让qemu直接去执行0x80000000位置的代码，所以在链接时需要指定代码段的起始地址是0x80000000，并且链接时需要汇编代码中的_start在0x80000000的位置。

```c
# qemu/hw/riscv/virt.c
static const MemMapEntry virt_memmap[] = {
    [VIRT_DEBUG] =        {        0x0,         0x100 },
    [VIRT_MROM] =         {     0x1000,        0xf000 },			# bootloader
    [VIRT_TEST] =         {   0x100000,        0x1000 },
    [VIRT_RTC] =          {   0x101000,        0x1000 },
    [VIRT_CLINT] =        {  0x2000000,       0x10000 },
    [VIRT_ACLINT_SSWI] =  {  0x2F00000,        0x4000 },
    [VIRT_PCIE_PIO] =     {  0x3000000,       0x10000 },
    [VIRT_IOMMU_SYS] =    {  0x3010000,        0x1000 },
    [VIRT_PLATFORM_BUS] = {  0x4000000,     0x2000000 },
    [VIRT_PLIC] =         {  0xc000000, VIRT_PLIC_SIZE(VIRT_CPUS_MAX * 2) },
    [VIRT_APLIC_M] =      {  0xc000000, APLIC_SIZE(VIRT_CPUS_MAX) },
    [VIRT_APLIC_S] =      {  0xd000000, APLIC_SIZE(VIRT_CPUS_MAX) },
    [VIRT_UART0] =        { 0x10000000,         0x100 },
    [VIRT_VIRTIO] =       { 0x10001000,        0x1000 },
    [VIRT_FW_CFG] =       { 0x10100000,          0x18 },
    [VIRT_FLASH] =        { 0x20000000,     0x4000000 },
    [VIRT_IMSIC_M] =      { 0x24000000, VIRT_IMSIC_MAX_SIZE },
    [VIRT_IMSIC_S] =      { 0x28000000, VIRT_IMSIC_MAX_SIZE },
    [VIRT_PCIE_ECAM] =    { 0x30000000,    0x10000000 },
    [VIRT_PCIE_MMIO] =    { 0x40000000,    0x40000000 },
    [VIRT_DRAM] =         { 0x80000000,           0x0 },			# kernel
};
```

## 4 汇编代码

```asm
#include "../c_code/include/platform.h"

    .equ    STACK_SIZE, 1024
    .global _start
    .text
_start:
    csrr t0, mhartid
    bnez t0, park
    la sp, stacks+STACK_SIZE
    j start_kernel

park:
    wfi
    j park

.balign 16
stacks:
    .skip STACK_SIZE * MAXNUM_CPU
    .end
```

关于start.S的一些解释：

* 汇编文件以S结尾，支持预处理；
* _start是默认的链接器入口；
* 这里的stacks其实是放在text段内的。

## 5 C函数

### 5-1 源代码

```c
void start_kernel(void)
{
    while (1)
    {
        /* code */
    }
}
```

### 5-2 头文件

```c
#ifndef __PLATFORM_H__
#define __PLATFORM_H__

#define MAXNUM_CPU 8

#endif
```

## 6 gdbinit

```shell
set disassemble-next-line on
layout asm
layout regs
b _start
target remote:1234
c
```

关于gdbinit的一些解释：

* disassemble-next-line自动显示下一条要执行的指令；
* layout asm和layout regs分别启动汇编代码和寄存器窗口；
* b _start在0x80000000处设置断点；
* 因为qemu在启动时加了-s选项，即在1234端口启动gdb服务器，故通过target remote:1234来连接gdb服务器；
* 因为qemu在启动时加了-S选项，即暂停CPU的运行，故而c能够让CPU运行直到断点。
