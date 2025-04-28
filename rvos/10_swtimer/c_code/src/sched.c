#include "../include/os.h"

#define MAX_TASKS 10
#define STACK_SIZE 1024

void switch_from_null_to_first(struct context *);
void switch_to(struct context *);

static int _top = 0;
static int _current = -1;

uint8_t __attribute__((aligned(16))) tasks_stack[MAX_TASKS][STACK_SIZE];
struct context tasks_ctx[MAX_TASKS];

void sched_init()
{
    asm volatile("csrw mscratch , x0" ::);
}

void run_os()
{
    if (!_top)
        panic("No Task has been created");
    else
        switch_from_null_to_first(&tasks_ctx[++_current]);
}

int task_create(void (*task_entry)(void))
{
    if (_top < MAX_TASKS)
    {
        tasks_ctx[_top].ra = (reg_t)task_entry;
        tasks_ctx[_top].sp = (reg_t)&tasks_stack[_top][STACK_SIZE];
        tasks_ctx[_top].epc = tasks_ctx[_top].ra;
        _top++;
        return 0;
    }
    else
        return -1;
}

void task_yield()
{
    *(ptr_t *)(CLINT_BASE + CLINT_MSIP_BASE + 4 * read_mhartid()) = 1;
}

void task_delay(volatile int count)
{
    count *= 5000;
    while (count--)
        ;
}

void switch_task()
{
    _current = (_current + 1) % _top;
    struct context *next = &tasks_ctx[_current];
    switch_to(next);
}