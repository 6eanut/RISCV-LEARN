# UART

## 1 思路

这里可以把qemu提供的virt虚拟机器想象成一个真实的开发板，通过-kernel可以将os.elf放在0x80000000的内存，并让qemu直接运行该内核代码。

在Licheepi 4A上烧录操作系统是通过串口线UART来完成的，即将开发板上的TX连接主机的RX，将开发板上的RX连接TX，地线相连，然后就可以利用fastboot工具将操作系统烧录到磁盘内。

对于现在这个实验，相当于用“魔法”把操作系统烧录到了磁盘(但其实并没有)，并且将开发板插上电源后，就能够运行该操作系统，但是没法观察执行的情况，即没有终端，故这里需要用到UART。

相当于是在内核代码中写了一段新的代码，即向外设UART控制器中的THR寄存器写数据，以达到在主机终端显示的效果，这里默认串口线连上了，只需要向UART的THR寄存器写数据，主机就能看到了。

## 2 UART

Universal Asynchronous Receiver and Transmitter有几个特点：

* 串行：数据只能一位一位发送，需要规定好波特率(一秒内发多少位数据)，每次传输数据的位数，是否包含奇偶校验位等，这些需要由UART协议来规定；
* 异步：只有数据线，没有时钟线，如何确定当前串口线上有数据？当串口线一直为高电平时为无数据？也由UART协议规定；
* 全双工：每一方都有TX和RX，发送和接受可以同时进行。

在qemu的virt虚拟机器中，UART在内存的0x10000000地址处，想要通过UART来传输数据到主机，就需要对其中寄存器写数据，具体模拟的是NS16550a硬件UART控制器。

```c
# qemu/hw/riscv/virt.c
static const MemMapEntry virt_memmap[] = {
    [VIRT_DEBUG] =        {        0x0,         0x100 },
    [VIRT_MROM] =         {     0x1000,        0xf000 },			# bootloader
    [VIRT_TEST] =         {   0x100000,        0x1000 },
    [VIRT_RTC] =          {   0x101000,        0x1000 },
    [VIRT_CLINT] =        {  0x2000000,       0x10000 },
    [VIRT_ACLINT_SSWI] =  {  0x2F00000,        0x4000 },
    [VIRT_PCIE_PIO] =     {  0x3000000,       0x10000 },
    [VIRT_IOMMU_SYS] =    {  0x3010000,        0x1000 },
    [VIRT_PLATFORM_BUS] = {  0x4000000,     0x2000000 },
    [VIRT_PLIC] =         {  0xc000000, VIRT_PLIC_SIZE(VIRT_CPUS_MAX * 2) },
    [VIRT_APLIC_M] =      {  0xc000000, APLIC_SIZE(VIRT_CPUS_MAX) },
    [VIRT_APLIC_S] =      {  0xd000000, APLIC_SIZE(VIRT_CPUS_MAX) },
    [VIRT_UART0] =        { 0x10000000,         0x100 },			# UART
    [VIRT_VIRTIO] =       { 0x10001000,        0x1000 },
    [VIRT_FW_CFG] =       { 0x10100000,          0x18 },
    [VIRT_FLASH] =        { 0x20000000,     0x4000000 },
    [VIRT_IMSIC_M] =      { 0x24000000, VIRT_IMSIC_MAX_SIZE },
    [VIRT_IMSIC_S] =      { 0x28000000, VIRT_IMSIC_MAX_SIZE },
    [VIRT_PCIE_ECAM] =    { 0x30000000,    0x10000000 },
    [VIRT_PCIE_MMIO] =    { 0x40000000,    0x40000000 },
    [VIRT_DRAM] =         { 0x80000000,           0x0 },			# kernel
};
```

较为关键的共有四个寄存器：

```c
#define UART_RHR 0    // 接收保持寄存器（读）
#define UART_THR 0    // 发送保持寄存器（写）
#define UART_IER 1    // 中断使能寄存器
#define UART_LSR 5    // 线路状态寄存器（DR=数据就绪）
```

## 3 实现

通过直接对内存中的0x10000000+reg来对UART设备的寄存器进行读写，寄存器位宽是8bits。

### 3-1 UART初始化

设置波特率、以及一次通信多少位的数据等。

```c
void uart_init()
{
    // 通过对IER寄存器写，关闭中断
    uart_write_reg(IER, 0x00);

    // 因为会有多个寄存器共享同一个地址空间，故该地址具体指哪个寄存器，需要由另一个寄存器的值来决定
    // lcr的第7位决定地址空间0x10000000和0x10000001所存的数据表示的是哪个寄存器
    // 波特率由16bit表示，由DLL和DLM组成
    uint8_t lcr = uart_read_reg(LCR);
    uart_write_reg(LCR, lcr | (1 << 7));
    uart_write_reg(DLL, 0x03);
    uart_write_reg(DLM, 0x00);

    // 这里设置一次传输的数据位宽是8bits
    lcr = 0;
    uart_write_reg(LCR, lcr | (3 << 0));
}
```

### 3-2 向TX写数据

通过轮询的方式，像THR寄存器写入要传输的数据。

```c
void uart_putc(char ch)
{
    // 不断查询Line Status Register寄存器的值，判断TX是否空闲，
    while ((uart_read_reg(LSR) & LSR_TX_IDLE) == 0)
        ;
    // 向Transmit Holding Register (write mode寄存器写入ch
    uart_write_reg(THR, ch);
}
```

然后uart_puts函数会根据字符串的长度循环调用uart_putc，在内核函数中调用uart_puts即可。
