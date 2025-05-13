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
    uint64_t entry; // ELF Header : e_entry
    uint64_t alloc; // guestP's heap now addr
    uint64_t base;  // guestP's heap base addr
} mmu_t;

void mmu_load_segment(mmu_t *, Elf64_Phdr_t *, int);

#endif