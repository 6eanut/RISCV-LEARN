#include "../include/rvemu.h"

void print_ElfHeader(Elf64_Ehdr_t *elf64_ehdr_ptr)
{
    printf("***Elf Header***\n");
    Elf64_Ehdr_t elf64_ehdr = *elf64_ehdr_ptr;
    printf("ei_mag          :   %x\n"
           "ei_class        :   %x\n"
           "ei_data         :   %x\n"
           "ei_version      :   %x\n"
           "ei_osabi        :   %x\n"
           "ei_abiversion   :   %x\n"
           "e_type          :   %hx\n"
           "e_machine       :   %hx\n"
           "e_version       :   %x\n"
           "e_entry         :   %lx\n"
           "e_phoff         :   %lx\n"
           "e_shoff         :   %lx\n"
           "e_flags         :   %x\n"
           "e_ehsize        :   %hx\n"
           "e_phentsize     :   %hx\n"
           "e_phnum         :   %hx\n"
           "e_shentsize     :   %hx\n"
           "e_shnum         :   %hx\n"
           "e_shstrndx      :   %hx\n",
           *(uint32_t *)elf64_ehdr.e_ident,
           elf64_ehdr.e_ident[4], elf64_ehdr.e_ident[5],
           elf64_ehdr.e_ident[6], elf64_ehdr.e_ident[7],
           elf64_ehdr.e_ident[8],
           elf64_ehdr.e_type, elf64_ehdr.e_machine,
           elf64_ehdr.e_version, elf64_ehdr.e_entry,
           elf64_ehdr.e_phoff, elf64_ehdr.e_shoff,
           elf64_ehdr.e_flags, elf64_ehdr.e_ehsize,
           elf64_ehdr.e_phentsize, elf64_ehdr.e_phnum,
           elf64_ehdr.e_shentsize, elf64_ehdr.e_shnum,
           elf64_ehdr.e_shstrndx);
}

void print_ElfProgramHeaderTableEnrty(Elf64_Phdr_t *elf64_phdr_ptr)
{
    printf("***Elf Program Header Table Entry***\n");
    Elf64_Phdr_t elf64_phdr = *elf64_phdr_ptr;
    printf(
        "Type        :  %x\n"
        "Offset      :  %lx\n"
        "FileSize    :  %lx\n"
        "VirtAddr    :  %lx\n"
        "MemSize     :  %lx\n"
        "PhysAddr    :  %lx\n"
        "Flags       :  %x\n"
        "Align       :  %lx\n",
        elf64_phdr.p_type,
        elf64_phdr.p_offset,
        elf64_phdr.p_filesz,
        elf64_phdr.p_vaddr,
        elf64_phdr.p_memsz,
        elf64_phdr.p_paddr,
        elf64_phdr.p_flags,
        elf64_phdr.p_align);
}

void readElf_PHTE(Elf64_Ehdr_t *elf64_ehdr_ptr, Elf64_Phdr_t *elf64_phdr_ptr, int index, FILE *file)
{
    if (fseek(file, elf64_ehdr_ptr->e_phoff + elf64_ehdr_ptr->e_phentsize * index, SEEK_SET) != 0)
        MYEXIT("fseek fail");

    if (fread(elf64_phdr_ptr, 1, elf64_ehdr_ptr->e_phentsize, file) != elf64_ehdr_ptr->e_phentsize)
        MYEXIT("fread fail");
}

void debug_readElf(char *program)
{
    int fd = open(program, O_RDONLY);
    if (fd == -1)
        MYEXIT("open fail");
    FILE *file = fdopen(fd, "rb");

    // for Elf Header
    uint8_t buffer_ElfHeader[sizeof(Elf64_Ehdr_t)];
    if (fread(buffer_ElfHeader, 1, sizeof(Elf64_Ehdr_t), file) != sizeof(Elf64_Ehdr_t))
    {
        printf("fread fail\n");
    }
    print_ElfHeader((Elf64_Ehdr_t *)buffer_ElfHeader);
    Elf64_Ehdr_t elf64_ehdr = *(Elf64_Ehdr_t *)buffer_ElfHeader;

    // for Program Header Table
    uint8_t buffer_ProgramHeaderTableEntry[elf64_ehdr.e_phentsize];
    for (int i = 0; i < elf64_ehdr.e_phnum; i++)
    {
        readElf_PHTE((Elf64_Ehdr_t *)buffer_ElfHeader, (Elf64_Phdr_t *)buffer_ProgramHeaderTableEntry, i, file);
        Elf64_Phdr_t elf64_phdr = *(Elf64_Phdr_t *)buffer_ProgramHeaderTableEntry;
        if (elf64_phdr.p_type == PT_LOAD)
            print_ElfProgramHeaderTableEnrty((Elf64_Phdr_t *)buffer_ProgramHeaderTableEntry);
    }
}