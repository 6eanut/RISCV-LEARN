.text
.global spin_lock_asm
spin_lock_asm:
    li t0, 1
    amoswap.w.aq a0, t0, (a0)
    ret

.global spin_unlock_asm
spin_unlock_asm:
    amoswap.w.rl x0, x0, (a0)
    ret

.end