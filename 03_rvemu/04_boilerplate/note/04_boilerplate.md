# Boilerplate

## 1 思路

## 1-1 加载程序到内存

我们从这里开始把rvemu模拟器叫做host program(hostP)，把rvprogram程序叫做guest program(guestP)。

模拟器就是读guestP中的信息，比如程序的入口(存储在ELF Header的e_entry)，要执行的指令和指令会操作的数据(存储在Program Header Table)；然后根据这些信息，来模拟执行guestP。

通过前面的两个实验ReadELF，已经可以成功读取到了这些信息，为了方便，需要把这些信息加载到hostP的内存地址空间中。除此之外，guestP中可能会动态分配heap空间，故而在hostP中需要考虑这一点，不妨将其跟在指令和数据后面。

## 1-2 框架

在将程序加载到内存之后，就可以开始设计模拟器的框架了：

* machine需要有一个state，来记录当前的gp寄存器和pc值；
* 指令分为32位和16位，这决定了pc的变化，这需要根据象限来决定；
* ecall触发的系统调用需要额外处理；
* 常规流程就是1.根据pc取指，2.对raw_inst译码获得inst，3.根据inst的类型来执行相应的函数，4.函数的执行会修改state中的gp寄存器和pc的值，5.返回1继续执行

## 2 实现

## 2-1 加载程序到内存

这部分主要是两个内容，第一个是程序的入口地址，第二个是程序的heap管理。

不妨定义一个内存管理单元mmu，记录了程序的入口地址，和heap的基址以及当前分配到哪个位置了(这里指的都是hostP中的地址空间)。

```c
typedef struct
{
    uint64_t entry; // ELF Header : e_entry
    uint64_t alloc; // guestP's heap now addr
    uint64_t base;  // guestP's heap base addr
} mmu_t;
```

然后就是借助mmap函数，将参数所指路径的文件加载到内存空间了：

```c
void mmu_load_segment(mmu_t *mmu, Elf64_Phdr_t *elf64_phdr, int fd)
{
    int prot = flags_to_prot(elf64_phdr->p_flags);
    uint64_t offset = ROUNDDOWN(elf64_phdr->p_offset);
    uint64_t addr = ROUNDDOWN(TO_HOST(elf64_phdr->p_vaddr));
    uint64_t length = ROUNDUP(elf64_phdr->p_filesz + TO_HOST(elf64_phdr->p_vaddr) - addr);
    uint64_t host_alloc = (uint64_t)mmap((void *)addr, length, prot, MAP_PRIVATE | MAP_FIXED, fd, offset);
    if (host_alloc != addr)
        MYEXIT(".text/.data mmap fail");

    uint64_t bss_length = ROUNDUP(elf64_phdr->p_memsz + TO_HOST(elf64_phdr->p_vaddr) - addr) - length;
    if (bss_length)
    {
        // for .data
        addr = ROUNDUP(addr + length);
        host_alloc = (uint64_t)mmap((void *)addr, bss_length, prot, MAP_ANONYMOUS | MAP_FIXED | MAP_PRIVATE, -1, 0);
        if (host_alloc != addr)
            MYEXIT(".bss mmap fail");
    }

    mmu->base = mmu->alloc = MAX(mmu->alloc, host_alloc + bss_length);
}
```

值得注意的是，mmap要求addr和offset参数都是页对齐(向下对齐)，同样其映射的基本单位也是页，故而length也可以提前设置成页对齐(向上对齐)。

映射最初只映射filesz大小的空间，因为这部分数据是有效的；如果检测到有部分memsz所指定的bss区域没有放在filesz所向上对齐的部分，那么需要额外mmap空间。

## 2-2 框架

在main函数中，加入一个死循环，循环内分为两部分，第一部分是执行普通指令，第二部分是处理ecall，即第一部分当执行到ecall时会退出，并将state中的exit_reason设置为相应ecall信息。

```c
typedef struct
{
    uint64_t pc;
    uint64_t gp_regs[32];
    enum exit_reason_t reason;
} state_t;

int main(int argc, char *argv[])
{
    // ...
    // framework
    while (true)
    {
        machine_step(&machine);

        // handle ecall
        MYEXIT("handle ecall");
    }
}
```

在执行普通指令部分，分为两部分，第一部分是执行一个basic block，第二部分是处理控制流，即branch或ecall，并作出相应的操作：

```c
enum exit_reason_t
{
    ecall,
    direct_branch,
    indirect_branch,
    num_reasons,
};

void machine_step(machine_t *machine)
{
    while (true)
    {
        interp_exec_bb(&machine->state);
        if (machine->state.reason == ecall)
            break;
    }
}
```

在执行基本块的函数中，程序会从pc位置取指raw_inst，然后译码为inst，然后根据inst的type来执行对应的函数，并在执行完毕后判断是否遇到了branch或ecall，如果没有就继续执行。

```c
void interp_exec_bb(state_t *state)
{
    inst_t inst = {0};
    while (true)
    {
        uint32_t raw_inst = *(uint32_t *)state->pc;
        inst_decode(&inst, raw_inst);
        funcs[inst.type](state, &inst);
        state->gp_regs[zero] = 0;

        if (!inst.goon)
            break;

        state->pc += inst.rvc ? 2 : 4;
    }
}
```

inst_decode会读取raw_inst的内容，并转换为inst格式的数据：

```c
typedef struct
{
    uint8_t rd;
    uint8_t rs1;
    uint8_t rs2;
    int32_t imm;
    bool rvc;
    enum inst_type_t type;
    bool goon;
} inst_t;

void inst_decode(inst_t *inst, uint32_t raw_inst)
{
    uint32_t quadrant = QUADRANT(raw_inst);
    switch (quadrant)
    {
    // convert raw_inst to inst
    case 0x0:
        MYEXIT("quadrant : 0x0");
        break;
    case 0x1:
        MYEXIT("quadrant : 0x1");
        break;
    case 0x2:
        MYEXIT("quadrant : 0x2");
        break;
    case 0x3:
        MYEXIT("quadrant : 0x3");
        break;
    default:
        break;
    }
}
```
