extern void uart_init();
extern void uart_puts(char *);

void start_kernel(void)
{
    uart_init();
    uart_puts("hello rvos\n");
    while (1)
    {
    }
}