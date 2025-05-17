#include "../include/rvemu.h"

void debug_reg(state_t *state)
{
    const char *gp_reg_names[] = {
        "zero", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
        "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
        "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
        "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"};

    // const char *fp_reg_names[] = {
    //     "ft0", "ft1", "ft2", "ft3", "ft4", "ft5", "ft6", "ft7",
    //     "fs0", "fs1", "fa0", "fa1", "fa2", "fa3", "fa4", "fa5",
    //     "fa6", "fa7", "fs2", "fs3", "fs4", "fs5", "fs6", "fs7",
    //     "fs8", "fs9", "fs10", "fs11", "ft8", "ft9", "ft10", "ft11"};

    printf("\nGeneral Purpose Registers:\n");
    printf("┌─────────┬────────────────────┬─────────┬────────────────────┐\n");
    for (int i = 0; i < num_gp_regs; i++)
    {
        printf("│ %-7s │ 0x%016lx ", gp_reg_names[i], state->gp_regs[i]);
        if ((i + 1) % 2 == 0)
            printf("│\n");
    }
    printf("└─────────┴────────────────────┴─────────┴────────────────────┘\n");

    // 打印浮点寄存器
    // printf("\nFloating Point Registers:\n");
    // printf("┌─────────┬────────────────────┬─────────┬────────────────────┐\n");
    // for (int i = 0; i < num_fp_regs; i++)
    // {
    //     printf("│ %-7s │ 0x%016lx ", fp_reg_names[i], state->fp_regs[i].l);
    //     if ((i + 1) % 2 == 0)
    //         printf("│\n");
    // }
    // printf("└─────────┴────────────────────┴─────────┴────────────────────┘\n");
}

void debug_inst(state_t *state, inst_t *inst)
{
    printf("inst debug : \n");
    printf("type : %s, "
           "rs1 : %hx, "
           "rs2 : %hx, "
           "rd : %hx, "
           "imm : %x, "
           "rvc : %d, "
           "csr : %hx, "
           "stop : %d\n",
           INST_NAME(inst->type),
           inst->rs1, inst->rs2, inst->rd, inst->imm,
           inst->rvc, inst->csr, inst->stop);
}