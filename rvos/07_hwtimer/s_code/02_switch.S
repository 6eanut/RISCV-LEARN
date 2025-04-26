#include "../c_code/include/ctx.h"

.text

.global switch_to
.balign 4
switch_to:
    csrrw t6, mscratch, t6
    beqz t6, init

    write_ctx_from_reg t6
    csrr t5, mscratch
    STORE t5, 30*REG_SIZE(t6)

init:
    csrw mscratch, a0
    mv t6, a0
    write_reg_from_ctx t6
    ret

.end