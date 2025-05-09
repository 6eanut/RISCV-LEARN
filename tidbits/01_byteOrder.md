# Byte Order

**思路：多字节数据的内存访问->字节序->磁盘数据的存储方式->应用层、操作系统层、通信层**

> 在内存中，地址为0x00000000~0x00000003的部分，分别存放着0x12，0x34，0x56，0x78；如果想要从地址0x00000002读取一个HalfWord大小的数据，那么读到的内容是地址0x00000002和0x00000003的内容，但是具体的值由字节序来决定，即0x5678？还是0x7856？

首先，Byte=8bit，HalfWord=16bit，Word=32bit；在RISC-V中，编址单位是字节，即一个地址对应的是8bit的数据。

通过上面的例子，我们知道，从内存中读一个多字节数据，是先读低地址后读高地址，但先读到的数据是数据的低位还是高位，这由字节序来决定，所以字节序针对的是多字节数据的内存读取问题。

字节序主要分为大端序(Big-Endian)和小端序(Little-Endian)，大端序指数据的高位先被读，即存放在内存的低地址部分；小端序指数据的低位先被读，即存放在内存的高地址部分。对应上面的例子，分别是0x5678和0x7856。

所以要点共两个：

* 从内存中读多字节数据，先读低地址后读高地址；
* RISC-V采用小端序，即先读到的数据是最终数据的低位部分。

---

针对磁盘中的数据，其存储方式由文件系统和文件格式来决定，文件系统指ext4、ntfs等，文件格式指jpeg、png等。

文件系统负责管理文件的存储位置和元数据，文件格式定义数据如何存储和解析，故后者也涉及字节序的问题。

文件系统是操作系统层，文件格式是应用层，而通信层还包括协议，例如http、nfs等，其主要处理不同系统或设备之间的交互，同样涉及字节序问题。

## 例子

这里有一个C语言程序：

```c
#include <stdio.h>
int main()
{
    int a = 0x12345678;
    printf("a: 0x%02x\n", a);
    printf("address of a: %p\n", (void *)&a);
    unsigned char *p = (unsigned char *)&a;
    for (int i = 0; i < sizeof(a); i++)
    {
        printf("address: %p, value: 0x%02x\n", p + i, *(p + i));
    }
    return 0;
}
```

经过编译和执行有：

```shell
# gcc byteOrder.c  -o byteOrder
# ./byteOrder
a: 0x12345678
address of a: 0x3fc872d30c
address: 0x3fc872d30c, value: 0x78
address: 0x3fc872d30d, value: 0x56
address: 0x3fc872d30e, value: 0x34
address: 0x3fc872d30f, value: 0x12
```

可以看到，这是小端序的存储方式。
