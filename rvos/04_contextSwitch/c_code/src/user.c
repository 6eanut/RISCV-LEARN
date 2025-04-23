#include "../include/os.h"

#define DELAY 1000

void user_task0(void)
{
    uart_puts("Task0 Created\n");
    while (1)
    {
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

void os_main()
{
    task_create(user_task0);
    task_create(user_task1);
    task_create(user_task2);
    task_create(user_task3);
}