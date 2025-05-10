#ifndef RVEMU_H
#define RVEMU_H

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#include "./elf.h"

#define MYEXIT(msg)          \
    do                       \
    {                        \
        printf("%s\n", msg); \
        exit(1);             \
    } while (0)

#endif