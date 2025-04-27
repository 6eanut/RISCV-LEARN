#include "../c_code/include/ctx.h"

.text

.global switch_from_null_to_first
.balign 4
switch_from_null_to_first:
    csrrw t6, mscratch, t6
    csrw mscratch, a0
    mv t6, a0
    write_reg_from_ctx t6
    ret

.global preemptive_switch
preemptive_switch:
    csrw mscratch, a0

    lw a0, 31*REG_SIZE(a0)
    ret

.global collaborative_switch
collaborative_switch:
    csrrw t6, mscratch, t6
    csrw mepc, ra
    write_ctx_from_reg t6

    csrw mscratch, a0

    mv t6, a0
    write_reg_from_ctx t6
    ret

.end