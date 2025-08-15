#include "../c_code/include/ctx.h"

.text

.global trap_vector
.balign 4
trap_vector:
    csrrw t6, mscratch, t6
    write_ctx_from_reg t6
    csrr t5, mscratch
    STORE t5, 30*REG_SIZE(t6)

    csrw mscratch, t6

    csrr a0, mepc
    csrr a1, mcause
    call trap_handler

    csrw mepc, a0

    csrr t6, mscratch
    write_reg_from_ctx t6

    mret

.end