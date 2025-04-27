#include "../include/os.h"

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

void enable_mie_msie()
{
    reg_t _mie;
    asm volatile(
        "csrr %[_mie], mie\n"
        : [_mie] "=r"(_mie)
        :);
    _mie |= (1 << 3);
    asm volatile(
        "csrw mie, %[_mie]\n"
        :
        : [_mie] "r"(_mie));
}

void clint_init()
{
    // init mtimecmp reg
    update_mtimecmp();
    
    // set mie.sie and mie.tie
    enable_mie_mtie();
    enable_mie_msie();
}