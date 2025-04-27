#include "../c_code/include/ctx.h"

.text

.global switch_from_null_to_first
.balign 4
switch_from_null_to_first:
    csrw mscratch, a0
    mv t6, a0
    write_reg_from_ctx t6
    ret

.global switch_to
switch_to:
    csrw mscratch, a0
    ret

.end