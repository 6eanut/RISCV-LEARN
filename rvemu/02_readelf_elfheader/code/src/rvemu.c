#include "../include/rvemu.h"
int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage : ./rvemu ./rvprogram\n");
        exit(1);
    }

    printf("hello rvemu\n");
    debug_read_elf_from_program(argv[1]);
    return 0;
}