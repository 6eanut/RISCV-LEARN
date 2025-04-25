#include "../include/os.h"

void switch_to(struct context *);

#define MAX_TASKS 10
#define STACK_SIZE 1024

static int _top = 0;
static int _current = -1;

uint8_t __attribute__((aligned(16))) tasks_stack[MAX_TASKS][STACK_SIZE];
struct context tasks_ctx[MAX_TASKS];

void sched_init()
{
    asm volatile("csrw mscratch , x0" ::);
}

void schedule()
{
    if (!_top)
        panic("No Task has been created");
    else
    {
        _current = (_current + 1) % _top;
        struct context *next = &tasks_ctx[_current];
        switch_to(next);
    }
}

int task_create(void (*task_entry)(void))
{
    if (_top < MAX_TASKS)
    {
        tasks_ctx[_top].ra = (reg_t)task_entry;
        tasks_ctx[_top].sp = (reg_t)&tasks_stack[_top][STACK_SIZE];
        _top++;
        return 0;
    }
    else
        return -1;
}

void task_yield()
{
    schedule();
}

void task_delay(volatile int count)
{
    count *= 5000;
    while (count--)
        ;
}