#include "../include/os.h"

#define MAX_SWTIMER_NUMS 10

extern uint32_t ticker;

struct swtimer
{
    void (*func)(void *);
    void *arg;
    uint32_t timeout;
};

struct swtimer swtimer_list[MAX_SWTIMER_NUMS];

void swtimer_init()
{
    for (int i = 0; i < MAX_SWTIMER_NUMS; ++i)
        swtimer_list[i].func = NULL;
}

uint32_t swtimer_create(void (*func)(void *), void *arg, uint32_t timeout)
{
    uint32_t flag = 1;
    for (int i = 0; (i < MAX_SWTIMER_NUMS) && flag; ++i)
        if (swtimer_list[i].func == NULL)
        {
            flag = 0;
            swtimer_list[i].func = func;
            swtimer_list[i].arg = arg;
            swtimer_list[i].timeout = ticker + timeout;
        }
    return flag;
}

void swtimer_delete(uint32_t index)
{
    swtimer_list[index].func = NULL;
}

void swtimer_check()
{
    for (int i = 0; i < MAX_SWTIMER_NUMS; ++i)
        if ((swtimer_list[i].func != NULL) && (ticker >= swtimer_list[i].timeout))
        {
            swtimer_list[i].func(swtimer_list[i].arg);
            swtimer_delete(i);
        }
}