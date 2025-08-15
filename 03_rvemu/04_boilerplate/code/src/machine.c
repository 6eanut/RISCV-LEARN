#include "../include/rvemu.h"

void machine_load_program(machine_t *machine, char *program)
{
    int fd = open(program, O_RDONLY);
    if (fd == -1)
        MYEXIT("open fail");
    FILE *file = fdopen(fd, "rb");

    // for guest_entry
    uint8_t buffer_ElfHeader[sizeof(Elf64_Ehdr_t)];
    if (fread(buffer_ElfHeader, 1, sizeof(Elf64_Ehdr_t), file) != sizeof(Elf64_Ehdr_t))
        MYEXIT("fread fail");
    Elf64_Ehdr_t elf64_ehdr = *(Elf64_Ehdr_t *)buffer_ElfHeader;
    machine->mmu.entry = TO_HOST(elf64_ehdr.e_entry);

    // for .text .data
    uint8_t buffer_ProgramHeaderEntry[elf64_ehdr.e_phentsize];
    for (int i = 0; i < elf64_ehdr.e_phnum; i++)
    {
        read_elf_phdr((Elf64_Ehdr_t *)buffer_ElfHeader, (Elf64_Phdr_t *)buffer_ProgramHeaderEntry, i, file);
        Elf64_Phdr_t elf64_phdr = *(Elf64_Phdr_t *)buffer_ProgramHeaderEntry;
        if (elf64_phdr.p_type == PT_LOAD)
            mmu_load_segment(&machine->mmu, &elf64_phdr, fd);
    }
    close(fd);

    machine->state.pc = machine->mmu.entry;
}

void machine_step(machine_t *machine)
{
    while (true)
    {
        interp_exec_bb(&machine->state);
        if (machine->state.reason == ecall)
            break;
    }
}