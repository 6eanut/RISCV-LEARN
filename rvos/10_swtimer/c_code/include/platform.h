#ifndef __PLATFORM_H__
#define __PLATFORM_H__

#define MAXNUM_CPU 8

#define UART0 0x10000000L

#define LENGTH_RAM 128*1024*1024

// UART's interrupt source id in PLIC is 10
#define UART0_IRQ 10


// PLIC
#define PLIC_BASE 0xc000000L
#define PLIC_ENABLE_BASE 0x2000
#define PLIC_ENABLE_STRIDE 0x80
#define PLIC_PRIORITY_BASE 0x00
// CONTEXT : threadhold, claim/complete
#define PLIC_CONTEXT_BASE 0x200000
#define PLIC_CONTEXT_STRIDE 0x1000
#define PLIC_CONTEXT_THREADHOLD_BASE 0x00
#define PLIC_CONTEXT_CLAIM_BASE 0x04
#define PLIC_CONTEXT_COMPLETE_BASE 0x04

// CLINT
#define CLINT_BASE 0x2000000L
#define CLINT_MTIME_BASE 0xBFF8
#define CLINT_MTIMECMP_BASE 0x4000
#define CLINT_MSIP_BASE 0x00
#define TIME_INTERVAL 0x08000000

#endif