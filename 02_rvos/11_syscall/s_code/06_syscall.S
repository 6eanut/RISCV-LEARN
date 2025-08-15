.text

.global getSyscallID
getSyscallID:
    mv a0, a7
    ret

.global syscall_get_mhartid
syscall_get_mhartid:
    li a7, 1
    ecall
    ret

.global syscall_get_mstatus
syscall_get_mstatus:
    li a7, 2
    ecall
    ret

.end