#include "../include/rvemu.h"

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage : ./rvemu ./rvprogram\n");
        exit(1);
    }

    machine_t machine = {0};

// printf("hello rvemu\n");
// debug_readElf(argv[1]);

// load program
#ifdef DEBUG
    printf("1-machine_load_program : \n");
#endif
    machine_load_program(&machine, argv[1]);
#ifdef DEBUG
    printf("entry           : %lx\n", machine.mmu.entry);
    printf("host_alloc      : %lx\n", machine.mmu.host_alloc);
    printf("guest_alloc     : %lx\n", machine.mmu.guest_alloc);
    printf("host_base       : %lx\n", machine.mmu.host_base);
    printf("\n\n");

    printf("2-machine_mmu_init : \n");
#endif
    machine_mmu_init(&machine, (uint64_t)argc, argv);
#ifdef DEBUG
    printf("host_alloc      : %lx\n", machine.mmu.host_alloc);
    printf("guest_alloc     : %lx\n", machine.mmu.guest_alloc);
    printf("host_base       : %lx\n", machine.mmu.host_base);
    printf("\n\n");

    printf("3-machine_step : \n\n\n");
#endif
    // framework
    while (true)
    {
        machine_step(&machine);

        uint64_t syscall_id = machine_get_gp_reg(&machine, a7);
#ifdef DEBUG
        printf("syscall_id = %lu\n", syscall_id);
#endif

        // handle ecall
        uint64_t ret = do_syscall(&machine, syscall_id);
        machine_set_gp_reg(&machine, a0, ret);
    }

    return 0;
}