#include "../include/os.h"

extern is_locked spin_lock_asm();
extern void spin_unlock_asm();

is_locked spin_lock(lock *cs_lock)
{
    return spin_lock_asm(cs_lock);
}

void spin_unlock(lock *cs_lock)
{
    spin_unlock_asm(cs_lock);
}