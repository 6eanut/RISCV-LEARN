#ifndef __PLATFORM_H__
#define __PLATFORM_H__

#define MAXNUM_CPU 8

#define UART0 0x10000000L

#define LENGTH_RAM 128*1024*1024

// UART's interrupt source id in PLIC is 10
#define UART0_IRQ 10
#endif