#define SYS_getmhartid 1
#define SYS_getmstatus 2

extern syscallID getSyscallID();

extern reg_t syscall_get_mhartid();
extern reg_t syscall_get_mstatus();

extern void do_syscall();