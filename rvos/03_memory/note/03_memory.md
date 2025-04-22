# Memory

## 1 思路

操作系统os.elf会被放在内存的0x80000000位置，这是一个很粗粒度的说法，因为os.elf有数据段、代码段等各种段，每个段放在内存的哪个位置？这个问题在之前是由riscv64-unknown-elf-ld来完成的，即它将各个.o文件的代码段和数据段等合并在.elf文件的代码段和数据段，并且指定了.elf文件的代码段和数据段应该放在virt虚拟机器的哪个物理位置。

在之前链接的过程中，通过-Ttext=0x80000000来将os.elf代码段放在指定的物理内存部分，通过qemu启动时的-bios none来跳过bios，直接执行0x80000000地址的代码，其余段的分配均有riscv64-unknown-elf-ld来完成。

但现在我们想自己管理内存，即决定每个.o文件的各段如何合并到.elf的各段中，以及.elf的各段应该在物理内存的哪个位置，这可以编写os.ld文件并且在链接时通过-T os.ld来指定，以此达到管理内存的效果。

os.ld的编写决定了各个段的映射，当os.elf编译完成后，数据段和代码段等大小都已经固定，但是堆，是在程序运行时动态分配的，故需要我们自己额外处理，类似于C语言中的malloc和free函数。

为了便于调试，还需要实现printf函数。字符串可以放入缓冲区直接打印，但变量的打印需要识别百分号，然后将变量的值放入缓冲区，等全部识别完成后，就可以借助uart来实现打印缓冲区的效果了。

## 2 实现

### 2-1 Linker Script

ld会通过linker script脚本来决定如何将.o文件中的各个段合并到.elf文件的各个段，并且会将.elf文件的各个段映射到真实的物理内存地址上。

```ld
#include "c_code/include/platform.h"

OUTPUT_ARCH("riscv")

ENTRY(_start)

MEMORY
{
    ram (wxa!ri) : ORIGIN = 0x80000000, LENGTH = LENGTH_RAM
}

SECTIONS
{
    .text : {
        PROVIDE(_text_start = .);
        *(.text .text.*)
        PROVIDE(_text_end = .);
    } > ram

    .rodata : {
        PROVIDE(_rodata_start = .);
        *(.rodata .rodata.*)
        PROVIDE(_rodata_end = .);
    } > ram

    .data : {
        . = ALIGN(4096);
        PROVIDE(_data_start = .);
        *(.sdata .sdata.*)
        *(.data .data.*)
        PROVIDE(_data_end = .);
    } > ram

    .bss : {
        PROVIDE(_bss_start = .);
        *(.sbss .sbss.*)
        *(.bss .bss.*)
        *(COMMON)
        PROVIDE(_bss_end = .);
    } > ram

    PROVIDE(_memory_start = ORIGIN(ram));
    PROVIDE(_memory_end = ORIGIN(ram) + LENGTH(ram));

    PROVIDE(_heap_start = _bss_end);
    PROVIDE(_heap_size = _memory_end - _heap_start);
}
```

关于linker script文件的一些解释：

* 因为该文件包含预处理语句#include，故而在链接之前需要用CC来对该文件做处理，编译选项为-E -P -x c：
  * -E：告诉编译器仅做预处理，展开宏、处理条件编译、处理头文件等；
  * -P：禁止预处理器生成行标记，使输出更干净；
  * -x c：让编译器把输入文件当作C源代码文件来处理；
* OUTPUT_ARCH来指定架构；
* ENTRY指定代码段的入口；
* MEMORY用来描述真实物理内存上内存区域的起始位置和大小，这里只关心ram，即0x80000000位置，大小默认为128MB；
* SECTIONS告诉链接器如何将input sections映射到output sections，以及如何将outputs sections放置在物理内存上；
* PROVIDE类似于赋值语句，在文件中定义符号，包含名字和地址；
* 最后在链接阶段指定该文件，即可直到ld做链接操作。

### 2-2 Heap Management

#### 2-2-1 组织方式

```c
void page_init()
{
    ptr_t _heap_start_aligned = _align_ptr(HEAP_START);
    uint32_t num_reserved_pages = LENGTH_RAM / (PAGE_SIZE * PAGE_SIZE);
    _alloc_start = _heap_start_aligned + num_reserved_pages * PAGE_SIZE;
    _num_pages = (HEAP_SIZE - (_heap_start_aligned - HEAP_START)) / PAGE_SIZE - num_reserved_pages;
    _alloc_end = _alloc_start + _num_pages * PAGE_SIZE;

    printf("LENGTH_RAM = %d\n", LENGTH_RAM);
    printf("HEAP_START = %p (aligned to %p), HEAP_SIZE = 0x%lx, \n"
           "num of reserved pages = %d, num of pages to be allocated for heap = %d\n",
           HEAP_START, _heap_start_aligned, HEAP_SIZE, num_reserved_pages, _num_pages);

    struct Page_index *pi = (struct Page_index *)HEAP_START;
    for (int i = 0; i < _num_pages; ++i)
    {
        _clear(pi);
        pi++;
    }

    printf("HEAP    :   %p -> %p\n", _alloc_start, _alloc_end);
    printf("BSS     :   %p -> %p\n", BSS_START, BSS_END);
    printf("DATA    :   %p -> %p\n", DATA_START, DATA_END);
    printf("RODATA  :   %p -> %p\n", RODATA_START, RODATA_END);
    printf("TEXT    :   %p -> %p\n", TEXT_START, TEXT_END);
}
```

一些说明：

* 在linker script文件中通过PROVIDE语句定义的符号可以在汇编代码和C源码中使用(通过链接)，故而可以得到堆的起始地址和大小，即内存中除了固定的那些段之外，其余部分都可以认为是堆空间，那么在明确了堆空间的地址范围之后，就可以进行管理了；
* 堆空间的分配和释放采用4KB为基本单位，每个页都有对应的元数据来记录该页是否被分配，如果需要用到的空间大于一个页，那么这n个页需要是连续分配的，故而元数据除了需要标记该页是否被分配之外，还需要记录该页是否是连续分配的最后一个页；
* 元数据所占的空间是一个大概的值，页所占的空间(即可分配的空间)是堆空间减去元数据所占的空间，并且页需要4KB对齐，但元数据不需要对齐，每个元数据项只需要8bit即可(其实2bit就行，但是8bit这样方便)。元数据所占的页数是RAM/(PAGE_SIZE*PAGE_SIZE)，按照这个进行计算，会发现元数据个数比可分配的页数多，故没问题；
* 确定好元数据所占空间和页所占空间后，需要对元数据的flag进行清空初始化，只需对前_num_pages个元数据清空即可。

#### 2-2-2 分配

```c
void *page_alloc(int npages)
{
    int found = 0;
    struct Page_index *pi = (struct Page_index *)HEAP_START;
    for (int i = 0; i <= _num_pages - npages; i++)
    {
        if (_is_free(pi))
        {
            found++;
            struct Page_index *pi_j = pi + 1;
            for (int j = 0; j < npages - 1; j++)
            {
                if (_is_free(pi_j))
                    pi_j++;
                else
                {
                    found = 0;
                    break;
                }
            }
            if (found)
            {
                for (struct Page_index *page = pi; page < pi_j; page++)
                    _set_flag(page, PAGE_TAKEN);
                _set_flag(pi_j, PAGE_LAST);
                return (void *)(_alloc_start + i * PAGE_SIZE);
            }
        }
        pi++;
    }
    return NULL;
}
```

一些说明：

* 分配页的思路就是，从可分配空间从头开始找空页，当找到一个空页后，顺着这个空页开始往后找连续的空页，看是否满足要分配的页数，如果连续的空页数小于要分配的页数，那么需要继续往后找；
* 如果找到了这么一块连续的空间，那么就写相应的元数据，并把页地址返回；
* 值得注意的是，pi和i必须是同增同减的，即该for循环遍历的是所有页，那么每次循环都会看下一个页，相应的元数据pi也要指向下一个。不允许存在当前页和元数据不对应的情况。

#### 2-2-3 释放

```c
void page_free(void *p)
{
    ptr_t page = (ptr_t)p;
    struct Page_index *pi = (struct Page_index *)(HEAP_START + (page - _alloc_start) / PAGE_SIZE);
    while (1)
    {
        _clear(pi);
        pi++;
        if (_is_last(pi))
            return;
    }
}
```

一些说明：

* 释放就是给定页地址，进行空间的释放，直到最后一个页；
* 首先根据页地址和可分配的页起始地址，计算出元数据所在的位置；
* 然后遍历元数据，如果被占用，则释放；如果当前元数据显示当前页是最后一个页，那么就结束释放。

### 2-3 printf

为啥要自己写printf，不能直接使用标准库里面的？在回答这个问题之前，先来解释一下为什么我们平常写的C语言程序，经过编译之后，可以直接执行printf等语句。

编译器经过cc1和as将C语言文件变为.o目标文件，然后通过collect2将该目标文件和libc.so文件进行链接，而后可以执行printf等函数。

但是在该实验中，链接过程只会把自己编写的C和S文件进行编译后链接，并不会链接libc.so文件，故而如果在C中直接用printf，则会报错。如果手动将libc.so链接进自己写的代码，也可以，但是这会导致操作系统代码量骤增，因为其包含了很多不需要的函数。

这里采用自己实现简易版printf，即只利用编译器支持的类型和操作来实现printf，像va_list、size_t、va_arg等，都是编译器内置的一些类型，故而可以直接使用，不需要链接其他动态库。

os.h文件额外include了stddef和stdarg两个头文件，这两个文件内容均是宏定义#define和类型定义typedef，故而不存在链接其他so库的情况。

## 3 总结

决定编译好的elf中的各个段放在真实物理内存的哪个位置，堆空间的分配和释放需要自己实现，其余部分是固定不变的，组织方式采用数组方式，另外printf也需要自己实现简易版。
