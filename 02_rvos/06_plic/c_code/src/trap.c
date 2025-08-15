#include "../include/os.h"

// MCAUSE = INTERRUPT[31] + Exception CODE[30:0]
// INTERRUPT
#define MCAUSE_MASK_INTERRUPT (reg_t)0x80000000
// Exception CODE
#define MCAUSE_MASK_ECODE (reg_t)0x7fffffff

extern void trap_vector();

void external_interrupt_handler()
{
    reg_t irq_id = plic_claim();
    switch (irq_id)
    {
    case UART0_IRQ:
        uart_external_interrupt_handler();
        /* code */
        break;

    default:
        printf("Unknown PLIC Interrupt Source ID: %p\n", irq_id);
        break;
    }

    if (irq_id)
        plic_complete(irq_id);
}

void trap_init()
{
    asm volatile("csrw mtvec, %[trap_vector]" : : [trap_vector] "r"(trap_vector));
    // int mtvec = read_mtvec();
    // printf("mtvec = %p\n", mtvec);
}

reg_t trap_handler(reg_t mepc, reg_t mcause)
{
    reg_t return_pc = mepc;
    reg_t cause_code = mcause & MCAUSE_MASK_ECODE;
    if (mcause & MCAUSE_MASK_INTERRUPT)
    {
        printf("Interruption Happened, Exception Code : %ld\n", cause_code);
        switch (cause_code)
        {
        case 3:
            uart_puts("Software Interruption\n");
            break;
        case 7:
            uart_puts("Timer Interruption\n");
            break;
        case 11:
            uart_puts("External Interruption\n");
            external_interrupt_handler();
            break;
        default:
            uart_puts("Unkonwn Interruption\n");
            break;
        }
    }
    else
    {
        printf("Exception Happened, Exception Code: %ld\n", cause_code);
        panic("What can I do");
    }
    return return_pc;
}
