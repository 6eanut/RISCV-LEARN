#include "../include/os.h"

#define DELAY 100000

lock cs_lock = 0;

void user_task0(void)
{
    uart_puts("Task0 Created\n");
    // printf("mepc = %lx, ra = %lx\n", mepc, ra);
    // reg_t mepc = read_mepc();
    // printf("mepc = %lx\n", mepc);
    // reg_t mstatus = read_mstatus();
    // printf("mstatus = %lx\n", mstatus);
    // int mtvec = read_mtvec();
    // printf("mtvec = %p\n", mtvec);
    reg_t mstatus = syscall_get_mstatus();
    printf("mstatus = %lx\n", mstatus);
    while (1)
    {
        // *(int *)0x00000000 = 1;
        uart_puts("Task0 Running\n");
        task_delay(DELAY);
        // task_yield();
    }
}

void user_task1(void)
{
    uart_puts("Task1 Created\n");
    while (1)
    {
        uart_puts("Task1 Running\n");
        task_delay(DELAY);
        uart_puts("Task1 gives up hart\n");
        task_yield();
        uart_puts("Task1 has hart back\n");
    }
}

void user_task2(void)
{
    uart_puts("Task2_With_Lock -- Created\n");
    while (1)
    {
        uart_puts("Task2_With_Lock -- Enter \n");
        while (spin_lock(&cs_lock))
        {
            uart_puts("Another Task Has The Lock, So Wait!\n");
            task_delay(DELAY);
        }
        uart_puts("Task2_With_Lock -- Enter The Critical Section, Get The Lock\n");
        for (int i = 0; i < 10; i++)
        {
            printf("Task2_With_Lock -- Operation %d Finished\n", i);
            task_delay(DELAY);
        }
        spin_unlock(&cs_lock);
        uart_puts("Task2_With_Lock -- Exit The Critical Section, Release The Lock\n");
        // task_yield();
    }
}

void user_task3(void)
{
    uart_puts("Task3_With_Lock -- Created\n");
    while (1)
    {
        // reg_t mhartid = read_mhartid(), tp = read_tp();
        // printf("mhartid = %p, tp = %p\n", mhartid, tp);
        uart_puts("Task3_With_Lock -- Enter\n");
        while (spin_lock(&cs_lock))
        {
            uart_puts("Another Task Has The Lock, So Wait!\n");
            task_delay(DELAY);
        }
        uart_puts("Task3_With_Lock -- Enter The Critical Section, Get The Lock\n");
        for (int i = 0; i < 10; i++)
        {
            printf("Task3_With_Lock -- Operation %d Finished\n", i);
            task_delay(DELAY);
        }
        spin_unlock(&cs_lock);
        uart_puts("Task3_With_Lock -- Exit The Critical Section, Release The Lock\n");
        task_delay(DELAY);
        // task_yield();
    }
}

void user_task4(void)
{
    uart_puts("Task4 Created\n");
    // reg_t mstatus = read_mstatus();
    // printf("mstatus = %lx\n", mstatus);
    // int mtvec = read_mtvec();
    // printf("mtvec = %p\n", mtvec);
    reg_t mhartid = syscall_get_mhartid();
    printf("mhartid = %lx\n", mhartid);
    while (1)
    {
        // *(int *)0x00000000 = 1;
        uart_puts("Task4 Running\n");
        task_delay(DELAY);
        // task_yield();
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

void swtimer_func_test(void *arg)
{
    printf("swtimer_func_test %d Start\n", arg);
    task_delay(DELAY);
    printf("swtimer_func_test %d End\n", arg);
}

void swtimer_user_task0()
{
    uart_puts("Swtimer_user_task0 Created\n");
    for (int i = 0; i < 20; i++)
    {
        int flag = swtimer_create(swtimer_func_test, (void *)i, i * 2);
        if (flag == 0)
            printf("Swtimer_task %d Created Successfully\n", i);
        else
            printf("Swtimer_task %d Created Failed\n", i);
    }
    while (1)
    {
        uart_puts("Swtimer_user_task0 Running\n");
        task_delay(DELAY);
    }
}

void os_main()
{
    // for debug
    // uart_puts("os_main start\n");
    task_create(debug_user_task0);
    task_create(debug_user_task1);
    // task_create(user_task0);
    // task_create(user_task4);
    // task_create(user_task4);
    // task_create(user_task1);
    // task_create(user_task2);
    // task_create(user_task3);
    // task_create(swtimer_user_task0);
}