# Hardware Timer

## 1 思路

中断分为本地中断和全局中断，前者包括软件中断和定时器中断，后者包括外部中断。本地中断的管理由CLINT(Core Local INTerrupt)来管理，其类似于PLIC，是和hart独立分开的，这一章聚焦于定时器中断，与其有关的寄存器有两个，分别是mtime和mtimecmp。

## 2 CLINT之Hardware Timer

CLINT类似于UART、PLIC等，对其寄存器的访问主要是基于物理内存：

```c
# qemu/hw/riscv/virt.c
static const MemMapEntry virt_memmap[] = {
    [VIRT_DEBUG] =        {        0x0,         0x100 },
    [VIRT_MROM] =         {     0x1000,        0xf000 },			# bootloader
    [VIRT_TEST] =         {   0x100000,        0x1000 },
    [VIRT_RTC] =          {   0x101000,        0x1000 },
    [VIRT_CLINT] =        {  0x2000000,       0x10000 },			# CLINT
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

### 2-1 mtime

系统全局唯一，所有hart共用一个mtimer，不论是rv32还是rv64，其都是64位寄存器。上电复位时，硬件将其设置为0。其访问通过：

```c
#define CLINT_BASE 0x2000000L
#define CLINT_MTIME_BASE 0xBFF8

reg_t mtime = *(ptr_t *)(CLINT_BASE + CLINT_MTIME_BASE);
```

### 2-2 mtimecmp

每个hart一个mtimecmp，也是64位，由软件管理，硬件不改变其值，所以需要在初始化阶段对其赋值。mtime会按照规定频率递增，每当mtime改变，会检测mtime是否大于等于mtimecmp，若满足，则发生定时器中断。

当然，这一切的发生需要将mie的mtie位置为1，使定时器中断使能。当定时器中断发生时，hart会设置mip的mtip位，当软件更新了mtimecmp值之后，硬件会将mip的mtip位清空。

ticker为软件维护的计数器，即mtime变更多少次，ticker变更一次。

```c
#define CLINT_MTIMECMP_BASE 0x4000
#define TIME_INTERVAL 10000000
static uint32_t ticker = 0;

void update_mtimecmp()
{
    ticker++;
    reg_t mtime = *(ptr_t *)(CLINT_BASE + CLINT_MTIME_BASE);
    *(ptr_t *)(CLINT_BASE + CLINT_MTIMECMP_BASE + 8 * read_mhartid()) = mtime + TIME_INTERVAL;

    printf("ticker = %lx\n", ticker);
}
```

### 2-3 使能timer interrupt enable

```c
void enable_mie_mtie()
{
    reg_t _mie;
    asm volatile(
        "csrr %[_mie], mie\n"
        : [_mie] "=r"(_mie)
        :);
    _mie |= (1 << 7);
    asm volatile(
        "csrw mie, %[_mie]\n"
        :
        : [_mie] "r"(_mie));
}
```

前面的外部中断，需要把外设、PLIC、virt的三级中断都使能，因为这里是本地中断，故而只需要将virt的中断使能即可。
