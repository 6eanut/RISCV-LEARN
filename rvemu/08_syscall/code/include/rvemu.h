#ifndef RVEMU_H
#define RVEMU_H

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdint.h>
#include <sys/mman.h>
#include <stdbool.h>
#include <unistd.h>
#include <assert.h>
#include <math.h>
#include <string.h>
#include <sys/stat.h>

#include "./reg.h"
#include "./elf.h"
#include "./mmu.h"
#include "./machine.h"
#include "./inst.h"
#include "./debug.h"
#include "./syscall.h"
#include "./interp.h"

// #define DEBUG

#define MYEXIT(msg)          \
    do                       \
    {                        \
        printf("%s\n", msg); \
        exit(1);             \
    } while (0)

#endif