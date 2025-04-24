#include "../include/os.h"

#define DELAY 1000

void user_task0(void)
{
    uart_puts("Task0 Created\n");
    // int mtvec = read_mtvec();
    // printf("mtvec = %p\n", mtvec);
    while (1)
    {
        *(int *)0x00000000 = 1;
        uart_puts("Task0 Running\n");
        task_delay(DELAY);
        task_yield();
    }
}

void user_task1(void)
{
    uart_puts("Task1 Created\n");
    while (1)
    {
        uart_puts("Task1 Running\n");
        task_delay(DELAY);
        task_yield();
    }
}

void user_task2(void)
{
    uart_puts("Task2 Created\n");
    while (1)
    {
        uart_puts("Task2 Running\n");
        task_delay(DELAY);
        task_yield();
    }
}

void user_task3(void)
{
    uart_puts("Task3 Created\n");
    while (1)
    {
        uart_puts("Task3 Running\n");
        task_delay(DELAY);
        task_yield();
    }
}

void debug_user_task0()
{
    uart_puts("debug_user_task0 start\n");
    int flag = 1;
    while (1)
    {
        if (flag++ == 1)
        {
            int t3 = read_t3(), t4 = read_t4();
            printf("t3 = %d, t4 = %d\n", t3, t4);

            write_t3(3);
            printf("t3:3\n");
            t3 = read_t3(), t4 = read_t4();
            printf("t3 = %d, t4 = %d\n", t3, t4);

            uart_puts("debug_user_task0 yield\n");
            task_delay(DELAY);
            task_yield();
            uart_puts("debug_user_task0 back\n");

            write_t4(4);
            printf("t4:4\n");
            t3 = read_t3(), t4 = read_t4();
            printf("t3 = %d, t4 = %d\n", t3, t4);
        }
        uart_puts("debug_user_task0 run\n");
        task_delay(DELAY);
        task_yield();
    }
}

void debug_user_task1()
{
    uart_puts("debug_user_task1 start\n");
    int flag = 1;
    while (1)
    {
        if (flag++ == 1)
        {
            int t3 = read_t3(), t4 = read_t4();
            printf("t3 = %d, t4 = %d\n", t3, t4);

            write_t3(6);
            printf("t3:6\n");
            t3 = read_t3(), t4 = read_t4();
            printf("t3 = %d, t4 = %d\n", t3, t4);

            uart_puts("debug_user_task1 yield\n");
            task_delay(DELAY);
            task_yield();
            uart_puts("debug_user_task1 back\n");

            write_t4(8);
            printf("t4:8\n");
            t3 = read_t3(), t4 = read_t4();
            printf("t3 = %d, t4 = %d\n", t3, t4);
        }
        uart_puts("debug_user_task1 run\n");
        task_delay(DELAY);
        task_yield();
    }
}

void os_main()
{
    // for debug
    uart_puts("os_main start\n");
    task_create(debug_user_task0);
    task_create(debug_user_task1);
    task_create(user_task0);
    task_create(user_task1);
    task_create(user_task2);
    task_create(user_task3);
}