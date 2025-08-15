#include "../include/os.h"

extern void uart_init();
extern void page_init();
// extern void page_test();
extern void sched_init();
extern void os_main();
extern void schedule();

void start_kernel(void)
{
    // uart
    uart_init();
    uart_puts("hello rvos\n");

    // memory
    page_init();
    // page_test();

    // context switch
    sched_init();
    os_main();
    schedule();

    while (1)
    {
    }
}