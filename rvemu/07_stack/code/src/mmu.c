#include "../include/rvemu.h"

void debug_mmap(uint64_t addr, uint64_t length, int prot, uint64_t offset, uint64_t host_alloc)
{
    printf("debug_mmap : \n");
    printf("addr        :   %lx\n", addr);
    printf("length      :   %lx\n", length);
    printf("prot        :   %d\n", prot);
    printf("offset      :   %lx\n", offset);
    printf("host_alloc  :   %lx\n", host_alloc);
}

static int flags_to_prot(Elf64_Word p_flags)
{
    return (p_flags & PF_R ? PROT_READ : 0) |
           (p_flags & PF_W ? PROT_WRITE : 0) |
           (p_flags & PF_X ? PROT_EXEC : 0);
}

// mmap : map<.text/.data, memory>
// void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset);
// ATTENTION : addr and offset must be page align
void mmu_load_segment(mmu_t *mmu, Elf64_Phdr_t *elf64_phdr, int fd)
{
    int prot = flags_to_prot(elf64_phdr->p_flags);
    uint64_t offset = ROUNDDOWN(elf64_phdr->p_offset);
    uint64_t addr = ROUNDDOWN(TO_HOST(elf64_phdr->p_vaddr));
    uint64_t length = ROUNDUP(elf64_phdr->p_filesz + TO_HOST(elf64_phdr->p_vaddr) - addr);
    uint64_t host_alloc = (uint64_t)mmap((void *)addr, length, prot, MAP_PRIVATE | MAP_FIXED, fd, offset);
    debug_mmap(addr, length, prot, offset, host_alloc);
    if (host_alloc != addr)
        MYEXIT(".text/.data mmap fail");

    uint64_t bss_length = ROUNDUP(elf64_phdr->p_memsz + TO_HOST(elf64_phdr->p_vaddr) - addr) - length;
    if (bss_length > 0)
    {
        // for .data
        addr = ROUNDUP(addr + length);
        uint64_t bss_host_alloc = (uint64_t)mmap((void *)addr, bss_length, prot, MAP_ANONYMOUS | MAP_FIXED | MAP_PRIVATE, -1, 0);
        debug_mmap(addr, bss_length, prot, 0, host_alloc);
        if (bss_host_alloc != addr)
            MYEXIT(".bss mmap fail");
    }
    mmu->host_alloc = mmu->host_base = MAX(mmu->host_alloc, host_alloc + ROUNDUP(elf64_phdr->p_memsz + TO_HOST(elf64_phdr->p_vaddr) - addr));
    mmu->guest_alloc = TO_GUEST(mmu->host_alloc);
}

// before machine_mmu_init : [ program | heap]
// after machine_mmu_init : [program | stack | argc argv envp auxv | heap]
uint64_t mmu_alloc(mmu_t *mmu, uint64_t size)
{
    uint64_t res = TO_HOST(mmu->guest_alloc);
    mmu->guest_alloc += size;
    if ((size >= 0) && ((TO_HOST(mmu->guest_alloc) > mmu->host_alloc)))
    {
        uint64_t addr = (uint64_t)mmap((void *)mmu->host_alloc, ROUNDUP(size), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE | MAP_FIXED, -1, 0);
        assert(addr == mmu->host_alloc);
        mmu->host_alloc += ROUNDUP(size);
        printf("mmu_alloc : \n");
        printf("addr    : %lx\n"
               "length  : %lx\n"
               "size    : %lx\n",
               mmu->host_alloc, ROUNDUP(size), size);
    }
    else if (size < 0 && ROUNDUP(TO_HOST(mmu->guest_alloc)) < mmu->host_alloc)
    {
        uint64_t len = mmu->host_alloc - ROUNDUP(TO_HOST(mmu->guest_alloc));
        mmu->host_alloc = ROUNDUP(TO_HOST(mmu->guest_alloc));
        if (munmap((void *)mmu->host_alloc, len) != 0)
            MYEXIT("munmap fail");
        mmu->host_alloc -= len;
    }
    assert(mmu->host_alloc >= mmu->host_base);
    return res;
}

void mmu_write(uint64_t addr, uint8_t *data, uint64_t size)
{
    printf("mmu_write : \n");
    printf("addr to write : %lx, size to write : %lu\n", addr, size);
    memcpy((void *)addr, (void *)data, (size_t)size);
}