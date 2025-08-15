#include "../include/rvemu.h"

void debug_mmap(uint64_t addr, uint64_t length, int prot, uint64_t offset, uint64_t host_alloc)
{
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
    if (bss_length)
    {
        // for .data
        addr = ROUNDUP(addr + length);
        host_alloc = (uint64_t)mmap((void *)addr, bss_length, prot, MAP_ANONYMOUS | MAP_FIXED | MAP_PRIVATE, -1, 0);
        debug_mmap(addr, bss_length, prot, 0, host_alloc);
        if (host_alloc != addr)
            MYEXIT(".bss mmap fail");
    }

    mmu->base = mmu->alloc = MAX(mmu->alloc, host_alloc + bss_length);
}
