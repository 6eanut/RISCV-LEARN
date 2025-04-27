#include "../include/os.h"

#define PLIC_BASE 0xc000000L
#define PLIC_ENABLE_BASE 0x2000
#define PLIC_ENABLE_STRIDE 0x80
#define PLIC_PRIORITY_BASE 0x00
// CONTEXT : threadhold, claim/complete
#define PLIC_CONTEXT_BASE 0x200000
#define PLIC_CONTEXT_STRIDE 0x1000
#define PLIC_CONTEXT_THREADHOLD_BASE 0x00
#define PLIC_CONTEXT_CLAIM_BASE 0x04
#define PLIC_CONTEXT_COMPLETE_BASE 0x04

void enable_mstatus_mie()
{
    reg_t _mstatus;
    asm volatile(
        "csrr %[_mstatus], mstatus\n"
        : [_mstatus] "=r"(_mstatus)
        :);
    _mstatus |= 0x8;
    asm volatile(
        "csrw mstatus, %[_mstatus]\n"
        :
        : [_mstatus] "r"(_mstatus));
}

void enable_mie_meie()
{
    reg_t _mie;
    asm volatile(
        "csrr %[_mie], mie\n"
        : [_mie] "=r"(_mie)
        :);
    _mie |= 0x800;
    asm volatile(
        "csrw mie, %[_mie]\n"
        :
        : [_mie] "r"(_mie));
}

void enable_plic_enable_uart0(reg_t hartid)
{
    *(ptr_t *)(PLIC_BASE + PLIC_ENABLE_BASE + PLIC_ENABLE_STRIDE * hartid + (UART0_IRQ / 32) * 4) = (1 << (UART0_IRQ % 32));
}

void set_plic_uart0_priority()
{
    *(ptr_t *)(PLIC_BASE + PLIC_PRIORITY_BASE + UART0_IRQ * 4) = 1;
}

void set_plic_threadhold(reg_t hartid)
{
    *(ptr_t *)(PLIC_BASE + PLIC_CONTEXT_BASE + PLIC_CONTEXT_THREADHOLD_BASE + hartid * PLIC_CONTEXT_STRIDE) = 0;
}

// platform-level interrupt controller
void plic_init()
{
    // printf("plic init start\n");
    // reg_t mstatus = read_mstatus(), mie = read_mie();
    // printf("mstatus = %p, mie = %p\n", mstatus, mie);

    set_plic_uart0_priority();
    // reg_t plic_uart0_priority = *(ptr_t *)(PLIC_BASE + PLIC_PRIORITY_BASE + UART0_IRQ * 4);
    // printf("plic_uart0_priority, %p : %p\n", (PLIC_BASE + PLIC_PRIORITY_BASE + UART0_IRQ * 4), plic_uart0_priority);

    enable_plic_enable_uart0(read_mhartid());
    // reg_t plic_enable = *(ptr_t *)(PLIC_BASE + PLIC_ENABLE_BASE + PLIC_ENABLE_STRIDE * read_mhartid() + (UART0_IRQ / 32) * 4);
    // printf("plic_enable, %p : %p\n", (PLIC_BASE + PLIC_ENABLE_BASE + PLIC_ENABLE_STRIDE * read_mhartid() + (UART0_IRQ / 32) * 4), plic_enable);

    set_plic_threadhold(read_mhartid());
    // reg_t plic_threadhold = *(ptr_t *)(PLIC_BASE + PLIC_CONTEXT_BASE + PLIC_CONTEXT_THREADHOLD_BASE + read_mhartid() * PLIC_CONTEXT_STRIDE);
    // printf("plic_threadhold, %p : %p\n", (PLIC_BASE + PLIC_CONTEXT_BASE + PLIC_CONTEXT_THREADHOLD_BASE + read_mhartid() * PLIC_CONTEXT_STRIDE), plic_threadhold);

    enable_mstatus_mie();
    // mstatus = read_mstatus(), mie = read_mie();
    // printf("mstatus = %p, mie = %p\n", mstatus, mie);

    enable_mie_meie();
    // mstatus = read_mstatus(), mie = read_mie();
    // printf("mstatus = %p, mie = %p\n", mstatus, mie);

    // printf("plic init end\n");
}

reg_t plic_claim()
{
    reg_t irq_id = *(ptr_t *)(PLIC_BASE + PLIC_CONTEXT_BASE + PLIC_CONTEXT_CLAIM_BASE + read_mhartid() * PLIC_CONTEXT_STRIDE);
    return irq_id;
}

void plic_complete(reg_t irq_id)
{
    *(ptr_t *)(PLIC_BASE + PLIC_CONTEXT_BASE + PLIC_CONTEXT_COMPLETE_BASE + read_mhartid() * PLIC_CONTEXT_STRIDE) = irq_id;
}