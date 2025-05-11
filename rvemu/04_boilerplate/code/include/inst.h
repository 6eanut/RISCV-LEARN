#ifndef INST_H
#define INST_H

enum inst_type_t
{
    inst_addi,
    num_insts,
};

enum gp_reg_t
{
    zero,
    ra,
    sp,
    gp,
    tp,
    t0,
    t1,
    t2,
    s0,
    s1,
    a0,
    a1,
    a2,
    a3,
    a4,
    a5,
    a6,
    a7,
    s2,
    s3,
    s4,
    s5,
    s6,
    s7,
    s8,
    s9,
    s10,
    s11,
    t3,
    t4,
    t5,
    t6,
    num_gp_regs,
};

typedef struct
{
    uint8_t rd;
    uint8_t rs1;
    uint8_t rs2;
    int32_t imm;
    bool rvc;
    enum inst_type_t type;
    bool goon;
} inst_t;

void inst_decode(inst_t *, uint32_t);

#endif