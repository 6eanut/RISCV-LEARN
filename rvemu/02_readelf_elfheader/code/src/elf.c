#include "../include/rvemu.h"

void debug_read_elf_from_program(char *program)
{
    int fd = open(program, O_RDONLY);
    if (fd == -1)
    {
        printf("open fail\n");
        exit(1);
    }

    FILE *file = fdopen(fd, "rb");
    uint8_t buffer[sizeof(Elf64_Ehdr_t)];

    if (fread(buffer, 1, sizeof(Elf64_Ehdr_t), file) != sizeof(Elf64_Ehdr_t))
    {
        printf("fread fail\n");
    }

    Elf64_Ehdr_t elf64_ehdr = *(Elf64_Ehdr_t *)buffer;

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