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
    printf("1-machine_load_program : \n");
    machine_load_program(&machine, argv[1]);
    printf("entry           : %lx\n", machine.mmu.entry);
    printf("host_alloc      : %lx\n", machine.mmu.host_alloc);
    printf("guest_alloc     : %lx\n", machine.mmu.guest_alloc);
    printf("host_base       : %lx\n", machine.mmu.host_base);
    printf("\n\n");

    printf("2-machine_mmu_init : \n");
    machine_mmu_init(&machine, (uint64_t)argc, argv);
    printf("host_alloc      : %lx\n", machine.mmu.host_alloc);
    printf("guest_alloc     : %lx\n", machine.mmu.guest_alloc);
    printf("host_base       : %lx\n", machine.mmu.host_base);
    printf("\n\n");

    printf("3-machine_step : \n\n\n");
    // framework
    while (true)
    {
        machine_step(&machine);

        // handle ecall
        MYEXIT("handle ecall");
    }

    return 0;
}