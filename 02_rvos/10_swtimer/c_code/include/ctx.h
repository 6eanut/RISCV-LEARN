#ifndef CTX_H
#define CTX_H

#define LOAD lw
#define STORE sw
#define CSRR csrr
#define CSRW csrw
#define REG_SIZE 4

.macro write_reg_from_ctx base
    LOAD t5, 31*REG_SIZE(\base)
    CSRW mepc, t5
    
    LOAD ra, 0*REG_SIZE(\base)
    LOAD sp, 1*REG_SIZE(\base)
    LOAD t0, 4*REG_SIZE(\base)
    LOAD t1, 5*REG_SIZE(\base)
    LOAD t2, 6*REG_SIZE(\base)
    LOAD s0, 7*REG_SIZE(\base)
    LOAD s1, 8*REG_SIZE(\base)
    LOAD a0, 9*REG_SIZE(\base)
    LOAD a1, 10*REG_SIZE(\base)
    LOAD a2, 11*REG_SIZE(\base)
    LOAD a3, 12*REG_SIZE(\base)
    LOAD a4, 13*REG_SIZE(\base)
    LOAD a5, 14*REG_SIZE(\base)
    LOAD a6, 15*REG_SIZE(\base)
    LOAD a7, 16*REG_SIZE(\base)
    LOAD s2, 17*REG_SIZE(\base)
    LOAD s3, 18*REG_SIZE(\base)
    LOAD s4, 19*REG_SIZE(\base)
    LOAD s5, 20*REG_SIZE(\base)
    LOAD s6, 21*REG_SIZE(\base)
    LOAD s7, 22*REG_SIZE(\base)
    LOAD s8, 23*REG_SIZE(\base)
    LOAD s9, 24*REG_SIZE(\base)
    LOAD s10, 25*REG_SIZE(\base)
    LOAD s11, 26*REG_SIZE(\base)
    LOAD t3, 27*REG_SIZE(\base)
    LOAD t4, 28*REG_SIZE(\base)
    LOAD t5, 29*REG_SIZE(\base)
    LOAD t6, 30*REG_SIZE(\base)
.endm

.macro write_ctx_from_reg base
    STORE ra, 0*REG_SIZE(\base)
    STORE sp, 1*REG_SIZE(\base)
    STORE t0, 4*REG_SIZE(\base)
    STORE t1, 5*REG_SIZE(\base)
    STORE t2, 6*REG_SIZE(\base)
    STORE s0, 7*REG_SIZE(\base)
    STORE s1, 8*REG_SIZE(\base)
    STORE a0, 9*REG_SIZE(\base)
    STORE a1, 10*REG_SIZE(\base)
    STORE a2, 11*REG_SIZE(\base)
    STORE a3, 12*REG_SIZE(\base)
    STORE a4, 13*REG_SIZE(\base)
    STORE a5, 14*REG_SIZE(\base)
    STORE a6, 15*REG_SIZE(\base)
    STORE a7, 16*REG_SIZE(\base)
    STORE s2, 17*REG_SIZE(\base)
    STORE s3, 18*REG_SIZE(\base)
    STORE s4, 19*REG_SIZE(\base)
    STORE s5, 20*REG_SIZE(\base)
    STORE s6, 21*REG_SIZE(\base)
    STORE s7, 22*REG_SIZE(\base)
    STORE s8, 23*REG_SIZE(\base)
    STORE s9, 24*REG_SIZE(\base)
    STORE s10, 25*REG_SIZE(\base)
    STORE s11, 26*REG_SIZE(\base)
    STORE t3, 27*REG_SIZE(\base)
    STORE t4, 28*REG_SIZE(\base)
    STORE t5, 29*REG_SIZE(\base)

    CSRR t5, mepc
    STORE t5, 31*REG_SIZE(\base)
.endm

#endif