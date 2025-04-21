# RVOS Startup

## 1 环境

大致过程：在x86架构上编写操作系统内核源码，经过交叉编译工具链编译成riscv32的elf文件，通过qemu模拟硬件运行该elf文件。

```shell
# 安装交叉编译工具链、调试工具以及qemu：
sudo apt install build-essential gcc make perl dkms git gcc-riscv64-unknown-elf gdb-multiarch qemu-system-misc
# 编写操作系统内核源码
...
# 交叉编译工具链编译


```
