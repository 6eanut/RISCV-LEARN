#ifndef __OH_H__
#define __OH_H__

#include "./platform.h"
#include "./types.h"

#include <stddef.h>
#include <stdarg.h>

extern void uart_putc(char);
extern void uart_puts(char *);

extern int printf(const char *, ...);
extern void panic(char *);

extern void task_yield();
extern void task_delay(volatile int);
extern int task_create(void (*)(void));

extern reg_t read_t3();
extern reg_t read_t4();
extern void write_t3();
extern void write_t4();
extern reg_t read_mtvec();
extern reg_t read_mscratch();
extern reg_t read_mhartid();
extern reg_t read_tp();
extern reg_t read_mstatus();
extern reg_t read_mie();
extern reg_t read_mip();

extern reg_t plic_claim();
extern void plic_complete(reg_t);

extern void enable_uart_receive_interrupt();
extern void uart_external_interrupt_handler();

extern void update_mtimecmp();

#endif