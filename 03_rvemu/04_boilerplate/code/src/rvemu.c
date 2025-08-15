#include "../include/rvemu.h"

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage : ./rvemu ./rvprogram\n");
        exit(1);
    }

    machine_t machine = {0};

    printf("hello rvemu\n");
    // debug_readElf(argv[1]);

    // load program
    machine_load_program(&machine, argv[1]);
    printf("entry   : %lx\n", machine.mmu.entry);
    printf("alloc   : %lx\n", machine.mmu.alloc);
    printf("base    : %lx\n", machine.mmu.base);

    // framework
    while (true)
    {
        machine_step(&machine);

        // handle ecall
        MYEXIT("handle ecall");
    }

    return 0;
}