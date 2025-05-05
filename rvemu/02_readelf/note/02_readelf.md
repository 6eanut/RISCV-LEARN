# ReadELF

## 0 思路

rvemu模拟器就是把一个RISC-V的程序rvprogram作为参数传递给rvemu这个X86的可执行程序，然后在rvemu里面去读rvprogram这个程序的二进制信息，根据ELF文件的规范提取出信息，而后做后续处理，以达到模拟RISC-V指令执行的效果。

故而第一步是需要知道ELF文件的格式。

## 1 ELF

在rvos中，使用riscv64-unknown-elf-gcc来进行编译，其用于开发操作系统等裸机程序；传给rvemu的程序rvprogram是一个linux用户空间的可执行程序，故而用riscv64-linux-gnu-gcc来编译，然后可以借助riscv64-linux-gnu-readelf来查看rvprogram的内容，使用qemu-riscv64来模拟运行rvprogram。

### 1-1 rvprogram

先写一个简单的C语言程序，然后用riscv64-linux-gnu-gcc进行编译得到rvprogram可执行程序，然后通过riscv64-linux-gnu-readelf来获得ELF Header的内容。

本实验的目的就是在rvemu里面能够读取ELF Header的内容，ELF-64 Object File Format见[此](./elf-64-gen.pdf)。

```txt
ELF Header:
  Magic:   7f 45 4c 46 02 01 01 00 00 00 00 00 00 00 00 00 
  Class:                             ELF64
  Data:                              2's complement, little endian
  Version:                           1 (current)
  OS/ABI:                            UNIX - System V
  ABI Version:                       0
  Type:                              DYN (Position-Independent Executable file)
  Machine:                           RISC-V
  Version:                           0x1
  Entry point address:               0x5b0
  Start of program headers:          64 (bytes into file)
  Start of section headers:          6704 (bytes into file)
  Flags:                             0x5, RVC, double-float ABI
  Size of this header:               64 (bytes)
  Size of program headers:           56 (bytes)
  Number of program headers:         10
  Size of section headers:           64 (bytes)
  Number of section headers:         28
  Section header string table index: 27
```

以上就是通过readelf工具得到的ELF Header信息，更多细节见rvprogram目录下的代码。

### 1-2 rvemu

rvprogram可执行程序的路径是rvemu的参数，通过该路径可以读取到该可执行程序的内容。

根据Specification在C语言层面定义Elf64_Ehdr_t，然后读取rvprogram的前sizeof(Elf64_Ehdr_t)个字节，并分别存储在内存中，通过打印来判断是否读取成功。

```c
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
```

这里是读取rvprogram程序的ELF Header的函数：

```c
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
```

然后运行，并和前面通过readelf工具得到的ELF Header做对比，发现无误：

```shell
$ make run
./rvemu ../rvprogram/rvprogram
hello rvemu
ei_mag          :   464c457f
ei_class        :   2
ei_data         :   1
ei_version      :   1
ei_osabi        :   0
ei_abiversion   :   0
e_type          :   3
e_machine       :   f3
e_version       :   1
e_entry         :   5b0
e_phoff         :   40
e_shoff         :   1a30
e_flags         :   5
e_ehsize        :   40
e_phentsize     :   38
e_phnum         :   a
e_shentsize     :   40
e_shnum         :   1c
e_shstrndx      :   1b
```
