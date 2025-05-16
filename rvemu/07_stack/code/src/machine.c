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
        if (machine->state.exit_reason == indirect_branch ||
            machine->state.exit_reason == direct_branch)
            continue;
        if (machine->state.exit_reason == ecall)
            break;
    }
}

// host memory : [ program | stack | argc argv envp auxv | heap]
void machine_mmu_init(machine_t *machine, int argc, char *argv[])
{
    uint64_t stack = mmu_alloc(&machine->mmu, STACK_SIZE);
    machine->state.gp_regs[sp] = stack + STACK_SIZE;

    machine->state.gp_regs[sp] -= 8; // auxv
    machine->state.gp_regs[sp] -= 8; // envp
    machine->state.gp_regs[sp] -= 8; // argv

    argc -= 1; // ./rvemu rvprogram
    for (int i = argc; i > 0; i--)
    {
        uint64_t len = strlen(argv[i]);
        uint64_t addr = mmu_alloc(&machine->mmu, len);
        mmu_write(addr, (uint8_t *)argv[i], len);
        machine->state.gp_regs[sp] -= 8;
        mmu_write(machine->state.gp_regs[sp], (uint8_t *)&addr, 8);
    }

    machine->state.gp_regs[sp] -= 8; // argc
    mmu_write(machine->state.gp_regs[sp], (uint8_t *)&argc, 8);
}