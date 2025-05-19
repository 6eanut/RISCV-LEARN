# Stack

## 1 思路

在boilerplate一节，rvemu将rvprogram的指令和数据放进了host内存，并用state的alloc成员变量指定了当前分配给rvprogram可以用的heap的其实空间。

但是rvprogram中的局部变量是存放在stack中的，故而需要实现stack的管理，这里决定把stack放在rvprogram的指令和数据后面，heap前面。

## 2 实现

为了更方便的管理空间，这里区分开host和guest，对于host的地址都是page对齐的，对于guest是实际真正存储到了哪个地址位置：

```c
typedef struct
{
    uint64_t entry;       // ELF Header : e_entry
    uint64_t host_alloc;  // page align
    uint64_t host_base;   // page align
    uint64_t guest_alloc; // no need to page align
} mmu_t;
```

mmu_alloc函数的作用是，首先更新guest_alloc（即真正用到了内存的哪个位置，该位置之前都是用过的，该位置之后都是空闲的），然后判断是否需要释放或者分配新的页，然后去做分配和释放工作，最后返回可用的初始地址。

什么时候会调用mmu_alloc？当想要写数据时，需要先分配一块空间，然后再往空间里写(memcpy)，故而mmap用来分配，memcpy用来写，munmap用来释放。

```c
void machine_mmu_init(machine_t *machine, int argc, char *argv[])
{
    uint64_t stack = mmu_alloc(&machine->mmu, STACK_SIZE);
    machine->state.gp_regs[sp] = stack + STACK_SIZE;

    machine->state.gp_regs[sp] -= 8; // auxv
    machine->state.gp_regs[sp] -= 8; // envp
    machine->state.gp_regs[sp] -= 8; // argv

    argc -= 1; // ./rvemu rvprogram
    for (int i = argc; i > 0; i--)
    {
        uint64_t len = strlen(argv[i]);
        uint64_t addr = mmu_alloc(&machine->mmu, len);
        mmu_write(addr, (uint8_t *)argv[i], len);
        machine->state.gp_regs[sp] -= 8;
        mmu_write(machine->state.gp_regs[sp], (uint8_t *)&addr, 8);
    }

    machine->state.gp_regs[sp] -= 8; // argc
    mmu_write(machine->state.gp_regs[sp], (uint8_t *)&argc, 8);
}

uint64_t mmu_alloc(mmu_t *mmu, uint64_t size)
{
    uint64_t res = TO_HOST(mmu->guest_alloc);
    mmu->guest_alloc += size;
    if ((size >= 0) && ((TO_HOST(mmu->guest_alloc) > mmu->host_alloc)))
    {
        uint64_t addr = (uint64_t)mmap((void *)mmu->host_alloc, ROUNDUP(size), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE | MAP_FIXED, -1, 0);
        assert(addr == mmu->host_alloc);
        mmu->host_alloc += ROUNDUP(size);
    }
    else if (size < 0 && ROUNDUP(TO_HOST(mmu->guest_alloc)) < mmu->host_alloc)
    {
        uint64_t len = mmu->host_alloc - ROUNDUP(TO_HOST(mmu->guest_alloc));
        mmu->host_alloc = ROUNDUP(TO_HOST(mmu->guest_alloc));
        if (munmap((void *)mmu->host_alloc, len) != 0)
            MYEXIT("munmap fail");
    }
    assert(mmu->host_alloc >= mmu->host_base);
    return res;
}
```

首先会分配一段栈空间，然后把argc、argv、envp、auxv放在stack的bottom，其中argv的具体值放在stack后面，heap前面，stack中存放的只是argv真实值的地址。

## 3 Debug

这一节实验写完之后，就可以开始模拟运行程序了，在最初会执行失败，需要通过打印指令解码和指令执行以及寄存器状态来调试，找到错误的地方，下面总结一下：

* inst_t中的stop意思是，如果当前需要跳转，即pc不是递增，那么其赋值为true；情况有2种，第一种就是J-type，第二种就是B-type且条件成立；
* 有些压缩指令仅存在于RV32，在RV64中，其对应的编码会代表其他指令，所以需要注意哪些指令是RV32中特有的，这在本实验中不应该实现，而应该实现相应的RV64；
* rvprogram需要是静态链接的可执行程序，即所有可执行代码都在elf文件中，并且为了后续实现syscall方便，采用的是newlib库；编译工具链这件事情主要关注的是架构、以及链接所采用的库；
* 在RISC-V中文文档里面对于jalr中有这样一句“若省略rd，则默认为x1”，起初误以为是当rd是0时，将其赋成x1，但其实不对，这里的意思针对的是阅读代码时的特点，和编码无关；这有助于编写汇编语言的程序员去写程序，像是语法糖，即在写jalr指令时，如果显示制定了rd，那么就是指定的，如果没有，那么就是x1；
* 为什么在state里面，分别记录了host和guest的地址，因为在寄存器中，不应该出现host的地址，而在栈的读写时(load和store)，用到的是host地址；即pc、sp等所有的寄存器，存放的地址都应该是guest地址，然后在load和store时转换成host地址。
