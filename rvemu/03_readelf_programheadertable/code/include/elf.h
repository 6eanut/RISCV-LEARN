#ifndef ELF_H
#define ELF_H

#include <stdint.h>

// ELF-64 Object File Format : https://uclibc.org/docs/elf-64-gen.pdf
typedef uint64_t Elf64_Addr;  // Unsigned Program Address
typedef uint64_t Elf64_Off;   // Unsigned FIle Offset
typedef uint16_t Elf64_Half;  // Unsigned Medium Integer
typedef uint32_t Elf64_Word;  // Unsigned Integer
typedef uint64_t Elf64_Xword; // Unsigned Long Integer

#define PT_LOAD 1   // Loadable Segment

// Elf Header
typedef struct
{
    unsigned char e_ident[16];
    Elf64_Half e_type;
    Elf64_Half e_machine;
    Elf64_Word e_version;
    Elf64_Addr e_entry;
    Elf64_Off e_phoff;
    Elf64_Off e_shoff;
    Elf64_Word e_flags;
    Elf64_Half e_ehsize;
    Elf64_Half e_phentsize;
    Elf64_Half e_phnum;
    Elf64_Half e_shentsize;
    Elf64_Half e_shnum;
    Elf64_Half e_shstrndx;
} Elf64_Ehdr_t;

// Program Header Table
typedef struct
{
    Elf64_Word p_type;
    Elf64_Word p_flags;
    Elf64_Off p_offset;
    Elf64_Addr p_vaddr;
    Elf64_Addr p_paddr;
    Elf64_Xword p_filesz;
    Elf64_Xword p_memsz;
    Elf64_Xword p_align;
} Elf64_Phdr_t;

void debug_readElf(char *);

#endif