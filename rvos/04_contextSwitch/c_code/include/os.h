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


#endif