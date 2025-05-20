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
