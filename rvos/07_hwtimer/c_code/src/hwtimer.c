#include "../include/os.h"

#define CLINT_BASE 0x2000000L
#define CLINT_MTIME_BASE 0xBFF8
#define CLINT_MTIMECMP_BASE 0x4000
#define TIME_INTERVAL 10000000

static uint32_t ticker = 0;

void update_mtimecmp()
{
    ticker++;
    reg_t mtime = *(ptr_t *)(CLINT_BASE + CLINT_MTIME_BASE);
    *(ptr_t *)(CLINT_BASE + CLINT_MTIMECMP_BASE + 8 * read_mhartid()) = mtime + TIME_INTERVAL;

    printf("ticker = %lx\n", ticker);
}

void enable_mie_mtie()
{
    reg_t _mie;
    asm volatile(
        "csrr %[_mie], mie\n"
        : [_mie] "=r"(_mie)
        :);
    _mie |= (1 << 7);
    asm volatile(
        "csrw mie, %[_mie]\n"
        :
        : [_mie] "r"(_mie));
}

void hwtimer_init()
{
    // init mtimecmp reg
    update_mtimecmp();

    // set mie
    enable_mie_mtie();
}