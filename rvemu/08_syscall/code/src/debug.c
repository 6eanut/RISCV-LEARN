#include "../include/rvemu.h"

const char *reg_names[] = {
    "zero", "ra", "sp", "gp", "tp",
    "t0", "t1", "t2", "s0", "s1",
    "a0", "a1", "a2", "a3", "a4", "a5", "a6", "a7",
    "s2", "s3", "s4", "s5", "s6", "s7", "s8", "s9", "s10", "s11",
    "t3", "t4", "t5", "t6"};

const char *inst_names[] = {
    "lui", "auipc", "jal", "jalr", "beq", "bne", "blt", "bge",
    "bltu", "bgeu", "lb", "lh", "lw", "lbu", "lhu", "sb", "sh", "sw",
    "addi", "slti", "sltiu", "xori", "ori", "andi", "slli", "srli",
    "srai", "add", "sub", "sll", "slt", "sltu", "xor", "srl", "sra",
    "or", "and", "fence", "fence_i", "ecall", "ebreak", "csrrw",
    "csrrs", "csrrc", "csrrwi", "csrrsi", "csrrci", "lwu", "ld",
    "sd", "addiw", "slliw", "srliw", "sraiw", "addw", "subw",
    "sllw", "srlw", "sraw", "mul", "mulh", "mulhsu", "mulhu",
    "div", "divu", "rem", "remu", "mulw", "divw", "divuw",
    "remw", "remuw", "flw", "fsw", "fmadd_s", "fmsub_s",
    "fnmsub_s", "fnmadd_s", "fadd_s", "fsub_s", "fmul_s",
    "fdiv_s", "fsqrt_s", "fsgnj_s", "fsgnjn_s", "fsgnjx_s",
    "fmin_s", "fmax_s", "fcvt_w_s", "fcvt_wu_s", "fmv_x_w",
    "feq_s", "flt_s", "fle_s", "fclass_s", "fcvt_s_w",
    "fcvt_s_wu", "fmv_w_x", "fcvt_l_s", "fcvt_lu_s",
    "fcvt_s_l", "fcvt_s_lu", "fld", "fsd", "fmadd_d",
    "fmsub_d", "fnmsub_d", "fnmadd_d", "fadd_d", "fsub_d",
    "fmul_d", "fdiv_d", "fsqrt_d", "fsgnj_d", "fsgnjn_d",
    "fsgnjx_d", "fmin_d", "fmax_d", "fcvt_s_d", "fcvt_d_s",
    "feq_d", "flt_d", "fle_d", "fclass_d", "fcvt_w_d",
    "fcvt_wu_d", "fcvt_d_w", "fcvt_d_wu", "fcvt_l_d",
    "fcvt_lu_d", "fmv_x_d", "fcvt_d_l", "fcvt_d_lu",
    "fmv_d_x"};

const char *fp_reg_names[] = {
    "ft0", "ft1", "ft2", "ft3", "ft4", "ft5", "ft6", "ft7",
    "fs0", "fs1", "fa0", "fa1", "fa2", "fa3", "fa4", "fa5",
    "fa6", "fa7", "fs2", "fs3", "fs4", "fs5", "fs6", "fs7",
    "fs8", "fs9", "fs10", "fs11", "ft8", "ft9", "ft10", "ft11"};

void debug_reg(state_t *state)
{

    printf("\nGeneral Purpose Registers:\n");
    printf("┌─────────┬────────────────────┬─────────┬────────────────────┐\n");
    for (int i = 0; i < num_gp_regs; i++)
    {
        printf("│ %-7s │ 0x%016lx ", reg_names[i], state->gp_regs[i]);
        if ((i + 1) % 2 == 0)
            printf("│\n");
    }
    printf("└─────────┴────────────────────┴─────────┴────────────────────┘\n");

    printf("\nFloating Point Registers:\n");
    printf("┌─────────┬────────────────────┬─────────┬────────────────────┐\n");
    for (int i = 0; i < num_fp_regs; i++)
    {
        printf("│ %-7s │ 0x%016lx ", fp_reg_names[i], state->fp_regs[i].l);
        if ((i + 1) % 2 == 0)
            printf("│\n");
    }
    printf("└─────────┴────────────────────┴─────────┴────────────────────┘\n");
}

// void debug_inst(state_t *state, inst_t *inst) {
//     printf("Instruction debug:\n");
//     printf("  Type: %s\n", inst_names[inst->type]);

//     bool is_float_inst = (inst->type >= inst_flw && inst->type <= inst_fmv_d_x);

//     if (is_float_inst) {
//         if (inst->rs1 != zero)
//             printf("  Source reg1: %s (%hx)\n", fp_reg_names[inst->rs1], inst->rs1);
//         if (inst->rs2 != zero)
//             printf("  Source reg2: %s (%hx)\n", fp_reg_names[inst->rs2], inst->rs2);
//         if (inst->rd != zero)
//             printf("  Destination reg: %s (%hx)\n", fp_reg_names[inst->rd], inst->rd);
//     } else {
//         if (inst->rs1 != zero)
//             printf("  Source reg1: %s (%hx)\n", reg_names[inst->rs1], inst->rs1);
//         if (inst->rs2 != zero)
//             printf("  Source reg2: %s (%hx)\n", reg_names[inst->rs2], inst->rs2);
//         if (inst->rd != zero)
//             printf("  Destination reg: %s (%hx)\n", reg_names[inst->rd], inst->rd);
//     }

//     if (inst->imm != 0)
//         printf("  Immediate: %d (0x%x)\n", inst->imm, inst->imm);

//     printf("  RVC: %s\n", inst->rvc ? "yes" : "no");
//     printf("  Stop: %s\n", inst->stop ? "true" : "false");
// }

void debug_inst(state_t *state, inst_t *inst) {
    printf("Instruction debug:\n");
    printf("  Type: %s\n", inst_names[inst->type]);

    bool is_float_inst = (inst->type >= inst_flw && inst->type <= inst_fmv_d_x);

    bool src_is_int = false;
    bool dest_is_float = false;

    if (is_float_inst) {
        if (inst->type == inst_flw || inst->type == inst_fld || 
            inst->type == inst_fmv_w_x || inst->type == inst_fmv_d_x ||
            inst->type == inst_fcvt_s_w || inst->type == inst_fcvt_s_wu ||
            inst->type == inst_fcvt_d_w || inst->type == inst_fcvt_d_wu ||
            inst->type == inst_fcvt_s_l || inst->type == inst_fcvt_s_lu ||
            inst->type == inst_fcvt_d_l || inst->type == inst_fcvt_d_lu) {
            src_is_int = true;
            dest_is_float = true;
        }
        else {
            dest_is_float = true;
        }
    }

    if (inst->rs1 != zero) {
        if (src_is_int) {
            printf("  Source reg1: %s (%hx)\n", reg_names[inst->rs1], inst->rs1);
        } else if (dest_is_float) {
            printf("  Source reg1: %s (%hx)\n", fp_reg_names[inst->rs1], inst->rs1);
        } else {
            printf("  Source reg1: %s (%hx)\n", reg_names[inst->rs1], inst->rs1);
        }
    }

    if (inst->rs2 != zero) {
        if (is_float_inst && !src_is_int) {
            printf("  Source reg2: %s (%hx)\n", fp_reg_names[inst->rs2], inst->rs2);
        } else {
            printf("  Source reg2: %s (%hx)\n", reg_names[inst->rs2], inst->rs2);
        }
    }

    if (inst->rd != zero) {
        if (dest_is_float) {
            printf("  Destination reg: %s (%hx)\n", fp_reg_names[inst->rd], inst->rd);
        } else {
            printf("  Destination reg: %s (%hx)\n", reg_names[inst->rd], inst->rd);
        }
    }

    if (inst->imm != 0)
        printf("  Immediate: %d (0x%x)\n", inst->imm, inst->imm);

    printf("  RVC: %s\n", inst->rvc ? "yes" : "no");
    printf("  Stop: %s\n", inst->stop ? "true" : "false");
}

void debug_syscall(machine_t *machine)
{
    printf("Syscall Debug:\n");
    printf("a0  =   %lu\n"
           "a1  =   %lu\n"
           "a2  =   %lu\n"
           "a3  =   %lu\n"
           "a4  =   %lu\n"
           "a5  =   %lu\n"
           "a6  =   %lu\n",
           machine->state.gp_regs[a0],
           machine->state.gp_regs[a1],
           machine->state.gp_regs[a2],
           machine->state.gp_regs[a3],
           machine->state.gp_regs[a4],
           machine->state.gp_regs[a5],
           machine->state.gp_regs[a6]);
}