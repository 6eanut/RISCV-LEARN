#include "../include/os.h"

extern void uart_init();
extern void page_init();
// extern void page_test();
extern void sched_init();
extern void os_main();
extern void run_os();
extern void trap_init();
extern void plic_init();
extern void hwtimer_init();

void start_kernel(void)
{
    // uart
    uart_init();
    uart_puts("hello rvos\n");

    // memory
    page_init();
    // page_test();

    // trap
    trap_init();

    // plic
    plic_init();

    // hardware timer
    hwtimer_init();

    // int mscratch = read_mscratch();
    // printf("mscratch = %p\n", mscratch);
    // context switch
    sched_init();
    os_main();
    run_os();

    while (1)
    {
    }
}