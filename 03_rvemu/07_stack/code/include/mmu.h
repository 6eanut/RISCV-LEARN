#ifndef MMU_H
#define MMU_H

#define GUEST_HOST_OFFSET 0x088800000000
#define TO_HOST(addr) (addr + GUEST_HOST_OFFSET)
#define TO_GUEST(addr) (addr - GUEST_HOST_OFFSET)

#define PAGE_SIZE 0x1000
#define ROUNDUP(addr) ((addr + PAGE_SIZE - 1) & -(PAGE_SIZE))
#define ROUNDDOWN(addr) (addr & -(PAGE_SIZE))

#define MAX(a, b) (a > b ? a : b)

typedef struct
{
    uint64_t entry;       // ELF Header : e_entry
    uint64_t host_alloc;  // page align
    uint64_t host_base;   // page align
    uint64_t guest_alloc; // no need to page align
} mmu_t;

void mmu_load_segment(mmu_t *, Elf64_Phdr_t *, int);

uint64_t mmu_alloc(mmu_t *, uint64_t);

void mmu_write(uint64_t, uint8_t *, uint64_t);

#endif