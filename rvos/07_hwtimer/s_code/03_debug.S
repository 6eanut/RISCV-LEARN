# ******for debug******
.text
.global read_t3
read_t3:
    mv a0, t3
    ret

.global write_t3
write_t3:
    mv t3, a0
    ret

.global read_t4
read_t4:
    mv a0, t4
    ret

.global write_t4
write_t4:
    mv t4, a0
    ret

.global read_mtvec
read_mtvec:
    csrr a0, mtvec
    ret

.global read_mscratch
read_mscratch:
    csrr a0, mscratch
    ret

.global read_mhartid
read_mhartid:
    csrr a0, mhartid
    ret

.global read_tp
read_tp:
    mv a0, tp
    ret

.global read_mstatus
read_mstatus:
    csrr a0, mstatus
    ret

.global read_mie
read_mie:
    csrr a0, mie
    ret

.global read_mip
read_mip:
    csrr a0, mip
    ret

.end