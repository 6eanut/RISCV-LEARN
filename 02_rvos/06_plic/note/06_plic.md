# PLIC

## 1 思路

前面实现了trap的初始化以及bottom half阶段，针对exception和interrupt可以根据mcause来打印出相应的信息。这个实验是在前面的基础上，实现对uart外部中断的识别，即qemu的virt虚拟机器通过串口线将uart设备连接到主机上，在主机的终端上按下键盘的某个字符，该字符信息会通过串口线和uart传送到virt的相应RHR寄存器上，我们预期os可以识别到RHR上来了字符，并把该字符读取，最终通过THR再打印回主机。

架构：IO主机——UART——PLIC——virt

## 2 PLIC

PLIC(Platform-Level Interrupt Controller)，一些解释：

在virt虚拟机器上，有多个hart和多个模拟IO外设，如果直接将它们互相连接，一方面是比较乱，无组织；另一方面是，如果多个外设同时发出中断请求，先处理哪个，需要在hart里面做判断。

为了简化这一过程，PLIC出现了，其一端连接hart们，一段连接IO外设们，它可以管理各个IO外设的中断优先级、开关状态等。PLIC的初始化需要os来完成，这一过程即修改PLIC的几个寄存器的值，这些寄存器通过内存物理地址来进行访问，与uart类似。

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
    [VIRT_PLIC] =         {  0xc000000, VIRT_PLIC_SIZE(VIRT_CPUS_MAX * 2) },	# PLIC
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

可以看到，上面的代码指明了PLIC的寄存器们在内存中的物理位置，其每个寄存器的含义以及相对位置也可以在代码中查明，这里举几个和初始化相关的：

```c
#define UART0_IRQ 10				# virt的PLIC目前支持53个外设，每个外设都有自己的ID，UART的ID是10
#define PLIC_BASE 0xc000000L			# 表明PLIC在物理内存中的基地址
#define PLIC_ENABLE_BASE 0x2000			# enable寄存器，每个hart有两个，每一位代表某个外设的中断使能
#define PLIC_ENABLE_STRIDE 0x80			# enable寄存器，stride的单位是hartid，即每个hart的enable寄存器相隔0x80个地址空间
#define PLIC_PRIORITY_BASE 0x00			# priority寄存器，每个外设都有其对应的priority寄存器，值为0~7，值越大优先级越高，优先级相同则比较ID，优先级为0表示禁用该外设中断
// CONTEXT : threadhold, claim/complete
#define PLIC_CONTEXT_BASE 0x200000		# Context包含threadhold寄存器和claim/complete寄存器
#define PLIC_CONTEXT_STRIDE 0x1000		# 每个hart的context字段相隔stride这么多地址空间
#define PLIC_CONTEXT_THREADHOLD_BASE 0x00	# threadhold寄存器，表示低于该值的priority不被考虑，每个hart一个threadhold
#define PLIC_CONTEXT_CLAIM_BASE 0x04		# claim用于获取需要处理中断的外设ID(PLIC会根据优先级自动选择)
#define PLIC_CONTEXT_COMPLETE_BASE 0x04		# complete和claim是一块寄存器，用于告知PLIC，当前ID的外设中断已处理完毕
```

所以，PLIC的初始化就是把每个hart的enable的使能打开，设置每个外设的priority，设置每个hart的threadhold；当然，除此之外，系统级别的与中断相关的csr还有mie和mstatus，这也需要打开：

```c
void plic_init()
{
    set_plic_uart0_priority();				# 设置每个外设的优先级

    enable_plic_enable_uart0(read_mhartid());		# 把每个hart的外设打开

    set_plic_threadhold(read_mhartid());		# 设置每个hart的threadhold

    enable_mstatus_mie();				# 设置csr的mstatus的mie

    enable_mie_meie();					# 设置csr的mie的meie
}
```

因为这里只选择实现外部中断中的uart中断，故优先级只设置了uart的priority，hart也只对0号hart的enable的uart外设做使能，threadhold初始化为零，mstatus里面记录了中断使能需要设置mie，mie里面记录了外部中断使能meie需要设置。

## 3 UART

在2 PLIC部分不仅对PLIC进行了中断初始化，还对os进行了中断初始化(mstatus和mie)，除此之外，还需要对uart的中断做初始化，即打开中断使能。IO主机——UART——PLIC——virt，目前是后两条线打通，但是必须把UART的中断使能打开，才能打开IO主机和UART之间的线。

```c
void enable_uart_receive_interrupt()
{
    reg_t ier = uart_read_reg(IER);
    uart_write_reg(IER, ier | 1);
}
```

## 4 UART外部中断处理

以上初始化完成后，操作系统运行，在IO主机键盘点击某个字符，此时virt就会识别到UART的外部中断了，接下来实现中断处理函数：

```c
void uart_external_interrupt_handler()
{
    // check LSR's RX is READY or not
    while ((uart_read_reg(LSR) & LSR_RX_READY) == 0)
        ;

    // read char from RHR
    char ch = uart_read_reg(RHR);

    uart_putc(ch);
    uart_putc('\n');
}
```

通过判断LSR的RX来的值RHR是否有数据到来，如果有，则读取，并通过写入THR来打印到IO主机。
