# Startup

在X86的Ubuntu上开发一个RISC-V模拟器，即该程序可以让一个在RISC-V架构上运行的程序在X86架构上运行。

## 0 环境准备

```shell
sudo apt install -y clang qemu-user gcc-riscv64-unknown-elf
```

因为本实验是用riscv64-linux-gnu-gcc来编译Linux用户态可执行程序，故而需要用qemu-user来模拟运行。

## 1 项目架构

目录结构如下：

```shell
$ tree
.
├── include
├── Makefile
├── obj
│   └── rvemu.o
├── rvemu
└── src
    └── rvemu.c

4 directories, 4 files
```

Makefile如下：

```makefile
CC=clang
CFLAGS=-O3 -Wall -Werror -Wimplicit-fallthrough

SRCS=$(wildcard src/*.c) 
HDRS=$(wildcard include/*.h)
OBJS=$(patsubst src/%.c, obj/%.o, $(SRCS))

rvemu:$(OBJS)
	$(CC) $(CFLAGS) -lm -o $@ $^

$(OBJS):obj/%.o:src/%.c $(HDRS)
	@mkdir -p $$(dirname $@)
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf rvemu obj/

.PHONY: clean
```
