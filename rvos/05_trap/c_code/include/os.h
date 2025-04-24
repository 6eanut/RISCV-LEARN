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

#endif