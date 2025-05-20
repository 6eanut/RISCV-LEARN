#include "../include/rvemu.h"
// sgnj fclass to be complete

static void func_lui(state_t *state, inst_t *inst) { UFUNC(imm); }
static void func_auipc(state_t *state, inst_t *inst) { UFUNC(imm + state->pc); }
static void func_jal(state_t *state, inst_t *inst)
{
    state->exit_reason = direct_branch;
    state->gp_regs[inst->rd] = state->pc + (inst->rvc ? 2 : 4);
    state->pc += (int64_t)inst->imm;
}
static void func_jalr(state_t *state, inst_t *inst)
{
    state->exit_reason = indirect_branch;
    state->gp_regs[inst->rd] = state->pc + (inst->rvc ? 2 : 4);
    state->pc = (state->gp_regs[inst->rs1] + (int64_t)inst->imm) & ~(uint64_t)1;
}
static void func_beq(state_t *state, inst_t *inst) { BFUNC(rs1 == rs2); }
static void func_bne(state_t *state, inst_t *inst) { BFUNC(rs1 != rs2); }
static void func_blt(state_t *state, inst_t *inst) { BFUNC((int64_t)rs1 < (int64_t)rs2); }
static void func_bge(state_t *state, inst_t *inst) { BFUNC((int64_t)rs1 >= (int64_t)rs2); }
static void func_bltu(state_t *state, inst_t *inst) { BFUNC((uint64_t)rs1 < (uint64_t)rs2); }
static void func_bgeu(state_t *state, inst_t *inst) { BFUNC((uint64_t)rs1 >= (uint64_t)rs2); }
static void func_lb(state_t *state, inst_t *inst) { IFUNC((int64_t)*(int8_t *)TO_HOST(rs1 + imm)); }
static void func_lh(state_t *state, inst_t *inst) { IFUNC((int64_t)*(int16_t *)TO_HOST(rs1 + imm)); }
static void func_lw(state_t *state, inst_t *inst)
{
    IFUNC((int64_t)*(int32_t *)TO_HOST(rs1 + imm));
#ifdef DEBUG
    printf("address to load : %lx, data to load : %lx\n", rs1 + imm, (int64_t)*(int32_t *)TO_HOST(rs1 + imm));
#endif
}
static void func_lbu(state_t *state, inst_t *inst) { IFUNC((int64_t)*(uint8_t *)TO_HOST(rs1 + imm)); }
static void func_lhu(state_t *state, inst_t *inst) { IFUNC((int64_t)*(uint16_t *)TO_HOST(rs1 + imm)); }
static void func_sb(state_t *state, inst_t *inst)
{
#ifdef DEBUG
    printf("address to store : %lx\n", (state->gp_regs[inst->rs1] + (int64_t)inst->imm));
#endif
    *(uint8_t *)TO_HOST(state->gp_regs[inst->rs1] + inst->imm) = (uint8_t)state->gp_regs[inst->rs2];
}
static void func_sh(state_t *state, inst_t *inst) { *(uint16_t *)TO_HOST(state->gp_regs[inst->rs1] + inst->imm) = (uint16_t)state->gp_regs[inst->rs2]; }
static void func_sw(state_t *state, inst_t *inst)
{
#ifdef DEBUG
    printf("address to store : %lx, data to store : %x\n", (state->gp_regs[inst->rs1] + (int64_t)inst->imm), (uint32_t)state->gp_regs[inst->rs2]);
#endif
    *(uint32_t *)TO_HOST(state->gp_regs[inst->rs1] + inst->imm) = (uint32_t)state->gp_regs[inst->rs2];
}
static void func_addi(state_t *state, inst_t *inst) { IFUNC(rs1 + imm); }
static void func_slti(state_t *state, inst_t *inst) { IFUNC((int64_t)rs1 < (int64_t)imm); }
static void func_sltiu(state_t *state, inst_t *inst) { IFUNC((uint64_t)rs1 < (uint64_t)imm); }
static void func_xori(state_t *state, inst_t *inst) { IFUNC(rs1 ^ imm); }
static void func_ori(state_t *state, inst_t *inst) { IFUNC(rs1 | imm); }
static void func_andi(state_t *state, inst_t *inst) { IFUNC(rs1 & (uint64_t)imm); }
static void func_slli(state_t *state, inst_t *inst) { IFUNC(rs1 << imm); }
static void func_srli(state_t *state, inst_t *inst) { IFUNC(rs1 >> imm); }
static void func_srai(state_t *state, inst_t *inst) { IFUNC((int64_t)rs1 >> (imm)); }
static void func_add(state_t *state, inst_t *inst) { RFUNC(rs1 + rs2); }
static void func_sub(state_t *state, inst_t *inst) { RFUNC(rs1 - rs2); }
static void func_sll(state_t *state, inst_t *inst) { RFUNC(rs1 << (rs2 & 0x3f)); }
static void func_slt(state_t *state, inst_t *inst) { RFUNC((int64_t)rs1 < (int64_t)rs2); }
static void func_sltu(state_t *state, inst_t *inst) { RFUNC((uint64_t)rs1 < (uint64_t)rs2); }
static void func_xor(state_t *state, inst_t *inst) { RFUNC(rs1 ^ rs2); }
static void func_srl(state_t *state, inst_t *inst) { RFUNC((uint64_t)rs1 >> (rs2 & 0x3f)); }
static void func_sra(state_t *state, inst_t *inst) { RFUNC((int64_t)rs1 >> (rs2 & 0x3f)); }
static void func_or(state_t *state, inst_t *inst) { RFUNC(rs1 | rs2); }
static void func_and(state_t *state, inst_t *inst) { RFUNC(rs1 & rs2); }
static void func_fence(state_t *state, inst_t *inst) { printf("fence !\n"); }
static void func_fence_i(state_t *state, inst_t *inst) { printf("fence.i !\n"); }
static void func_ecall(state_t *state, inst_t *inst)
{
    state->pc += inst->rvc ? 2 : 4;
    state->exit_reason = ecall;
#ifdef DEBUG
    printf("ecall !\n");
#endif
}
static void func_ebreak(state_t *state, inst_t *inst) { printf("ebreak !\n"); }
static void func_csrrw(state_t *state, inst_t *inst) { printf("CSR Instruction ! Do Nothing !\n"); }
static void func_csrrs(state_t *state, inst_t *inst) { printf("CSR Instruction ! Do Nothing !\n"); }
static void func_csrrc(state_t *state, inst_t *inst) { printf("CSR Instruction ! Do Nothing !\n"); }
static void func_csrrwi(state_t *state, inst_t *inst) { printf("CSR Instruction ! Do Nothing !\n"); }
static void func_csrrsi(state_t *state, inst_t *inst) { printf("CSR Instruction ! Do Nothing !\n"); }
static void func_csrrci(state_t *state, inst_t *inst) { printf("CSR Instruction ! Do Nothing !\n"); }
static void func_lwu(state_t *state, inst_t *inst) { IFUNC((int64_t)*(uint32_t *)TO_HOST(rs1 + imm)); }
static void func_ld(state_t *state, inst_t *inst)
{
#ifdef DEBUG
    int64_t imm_debug = (int64_t)inst->imm;
    uint64_t rs1_debug = state->gp_regs[inst->rs1];
    printf("address to load : %lx\n", TO_HOST(imm_debug + rs1_debug));
#endif
    IFUNC(*(int64_t *)TO_HOST(rs1 + imm));
}
static void func_sd(state_t *state, inst_t *inst)
{
#ifdef DEBUG
    printf("address to store : %lx, data to store : %lx\n", (state->gp_regs[inst->rs1] + (int64_t)inst->imm), (uint64_t)state->gp_regs[inst->rs2]);
#endif
    *(uint64_t *)TO_HOST(state->gp_regs[inst->rs1] + inst->imm) = (uint64_t)state->gp_regs[inst->rs2];
}
static void func_addiw(state_t *state, inst_t *inst) { IFUNC((int64_t)(int32_t)(rs1 + imm)); }
static void func_slliw(state_t *state, inst_t *inst) { IFUNC((int64_t)(uint32_t)(rs1 << imm)); }
static void func_srliw(state_t *state, inst_t *inst) { IFUNC((int64_t)((uint32_t)rs1 >> imm)); }
static void func_sraiw(state_t *state, inst_t *inst) { IFUNC((uint64_t)(int32_t)rs1 >> (imm)); }
static void func_addw(state_t *state, inst_t *inst) { RFUNC((int64_t)(int32_t)(rs1 + rs2)); }
static void func_subw(state_t *state, inst_t *inst) { RFUNC((int64_t)(int32_t)(rs1 - rs2)); }
static void func_sllw(state_t *state, inst_t *inst) { RFUNC((int64_t)((uint32_t)rs1 << (rs2 & 0x1f))); }
static void func_srlw(state_t *state, inst_t *inst) { RFUNC((int64_t)((uint32_t)rs1 >> (rs2 & 0x1f))); }
static void func_sraw(state_t *state, inst_t *inst) { RFUNC((uint64_t)(int32_t)rs1 >> (rs2 & 0x1f)); }
static void func_mul(state_t *state, inst_t *inst) { RFUNC(rs1 * rs2); }
static void func_mulh(state_t *state, inst_t *inst)
{
    __int128_t rs1 = (__int128_t)state->gp_regs[inst->rs1];
    __int128_t rs2 = (__int128_t)state->gp_regs[inst->rs2];
    __int128_t rd = rs1 * rs2;
    state->gp_regs[inst->rd] = *((uint64_t *)(&rd) + 1);
#ifdef DEBUG
    printf("rd_low  =   %lx, rd_high    =   %lx\n", *(uint64_t *)(&rd), *((uint64_t *)(&rd) + 1));
    printf("rs1     =   %lx\n"
           "rs2     =   %lx\n"
           "rd      =   %lx\n",
           state->gp_regs[inst->rs1], state->gp_regs[inst->rs2], state->gp_regs[inst->rd]);
#endif
}
static void func_mulhsu(state_t *state, inst_t *inst)
{
    __int128_t rs1 = (__int128_t)(__uint128_t)state->gp_regs[inst->rs1];
    __int128_t rs2 = (__int128_t)state->gp_regs[inst->rs2];
    __int128_t rd = rs1 * rs2;
    state->gp_regs[inst->rd] = *((uint64_t *)(&rd) + 1);
}
static void func_mulhu(state_t *state, inst_t *inst)
{
    __uint128_t rs1 = (__uint128_t)state->gp_regs[inst->rs1];
    __uint128_t rs2 = (__uint128_t)state->gp_regs[inst->rs2];
    __uint128_t rd = rs1 * rs2;
    state->gp_regs[inst->rd] = *((uint64_t *)(&rd) + 1);
#ifdef DEBUG
    printf("rd_low  =   %lx, rd_high    =   %lx\n", *(uint64_t *)(&rd), *((uint64_t *)(&rd) + 1));
    printf("rs1     =   %lx\n"
           "rs2     =   %lx\n"
           "rd      =   %lx\n",
           state->gp_regs[inst->rs1], state->gp_regs[inst->rs2], state->gp_regs[inst->rd]);
#endif
}
static void func_div(state_t *state, inst_t *inst) { RFUNC(((int64_t)rs1 / (int64_t)rs2)); }
static void func_divu(state_t *state, inst_t *inst) { RFUNC(((uint64_t)rs1 / (uint64_t)rs2)); }
static void func_rem(state_t *state, inst_t *inst) { RFUNC((int64_t)rs1 % (int64_t)rs2); }
static void func_remu(state_t *state, inst_t *inst) { RFUNC((uint64_t)rs1 % (uint64_t)rs2); }
static void func_mulw(state_t *state, inst_t *inst) { RFUNC((int64_t)(uint32_t)(rs1 * rs2)); }
static void func_divw(state_t *state, inst_t *inst) { RFUNC(((int64_t)((int32_t)rs1 / (int32_t)rs2))); }
static void func_divuw(state_t *state, inst_t *inst) { RFUNC(((int64_t)((uint32_t)rs1 / (uint32_t)rs2))); }
static void func_remw(state_t *state, inst_t *inst) { RFUNC(((int64_t)(int32_t)rs1 % (int32_t)rs2)); }
static void func_remuw(state_t *state, inst_t *inst) { RFUNC(((int64_t)(uint32_t)rs1 % (uint32_t)rs2)); }
// | ((uint64_t)-1 << 32);
static void func_flw(state_t *state, inst_t *inst) { state->fp_regs[inst->rd].l = *(uint32_t *)TO_HOST(state->gp_regs[inst->rs1] + (int64_t)inst->imm) | ((uint64_t)-1 << 32); }
static void func_fsw(state_t *state, inst_t *inst) { *(uint32_t *)TO_HOST(state->gp_regs[inst->rs1] + (int64_t)inst->imm) = (uint32_t)state->fp_regs[inst->rs2].l; }
static void func_fmadd_s(state_t *state, inst_t *inst) { R4FUNC_S(rs1 * rs2 + rs3); }
static void func_fmsub_s(state_t *state, inst_t *inst) { R4FUNC_S(rs1 * rs2 - rs3); }
static void func_fnmsub_s(state_t *state, inst_t *inst) { R4FUNC_S(-(rs1 * rs2) + rs3); }
static void func_fnmadd_s(state_t *state, inst_t *inst) { R4FUNC_S(-(rs1 * rs2) - rs3); }
static void func_fadd_s(state_t *state, inst_t *inst) { RFUNC_F_S(rs1 + rs2); }
static void func_fsub_s(state_t *state, inst_t *inst) { RFUNC_F_S(rs1 - rs2); }
static void func_fmul_s(state_t *state, inst_t *inst) { RFUNC_F_S(rs1 * rs2); }
static void func_fdiv_s(state_t *state, inst_t *inst) { RFUNC_F_S(rs1 / rs2); }
static void func_fsqrt_s(state_t *state, inst_t *inst) { RFUNC_F_S(sqrtf(rs1)); }
static void func_fsgnj_s(state_t *state, inst_t *inst) { printf("fsgn !\n"); }
static void func_fsgnjn_s(state_t *state, inst_t *inst) { printf("fsgn !\n"); }
static void func_fsgnjx_s(state_t *state, inst_t *inst) { printf("fsgn !\n"); }
static void func_fmin_s(state_t *state, inst_t *inst) { RFUNC_F_S(rs1 < rs2 ? rs1 : rs2); }
static void func_fmax_s(state_t *state, inst_t *inst) { RFUNC_F_S(rs1 > rs2 ? rs1 : rs2); }
static void func_fcvt_w_s(state_t *state, inst_t *inst) { state->gp_regs[inst->rd] = (int64_t)(int32_t)state->fp_regs[inst->rs1].s; }
// (int64_t)(int32_t)(uint32_t)state->fp_regs[inst->rs1].s;
static void func_fcvt_wu_s(state_t *state, inst_t *inst) { state->gp_regs[inst->rd] = (int64_t)(int32_t)(uint32_t)state->fp_regs[inst->rs1].s; }
static void func_fmv_x_w(state_t *state, inst_t *inst) { state->gp_regs[inst->rd] = (int64_t)(int32_t)state->fp_regs[inst->rs1].w; }
static void func_feq_s(state_t *state, inst_t *inst) { RFUNC_F_S_L(rs1 == rs2); }
static void func_flt_s(state_t *state, inst_t *inst) { RFUNC_F_S_L(rs1 < rs2); }
static void func_fle_s(state_t *state, inst_t *inst) { RFUNC_F_S_L(rs1 <= rs2); }
static void func_fclass_s(state_t *state, inst_t *inst) { printf("fclass_s !\n"); }
static void func_fcvt_s_w(state_t *state, inst_t *inst) { state->fp_regs[inst->rd].s = (float)(int32_t)state->gp_regs[inst->rs1]; }
static void func_fcvt_s_wu(state_t *state, inst_t *inst) { state->fp_regs[inst->rd].s = (float)(uint32_t)state->gp_regs[inst->rs1]; }
static void func_fmv_w_x(state_t *state, inst_t *inst) { state->fp_regs[inst->rd].w = (uint32_t)state->gp_regs[inst->rs1]; }
static void func_fcvt_l_s(state_t *state, inst_t *inst) { state->gp_regs[inst->rd] = (int64_t)state->fp_regs[inst->rs1].s; }
static void func_fcvt_lu_s(state_t *state, inst_t *inst) { state->gp_regs[inst->rd] = (uint64_t)state->fp_regs[inst->rs1].s; }
static void func_fcvt_s_l(state_t *state, inst_t *inst) { state->fp_regs[inst->rd].s = (float)(int64_t)state->gp_regs[inst->rs1]; }
static void func_fcvt_s_lu(state_t *state, inst_t *inst) { state->fp_regs[inst->rd].s = (float)(uint64_t)state->gp_regs[inst->rs1]; }
static void func_fld(state_t *state, inst_t *inst) { state->fp_regs[inst->rd].l = *(uint64_t *)TO_HOST(state->gp_regs[inst->rs1] + (int64_t)inst->imm); }
static void func_fsd(state_t *state, inst_t *inst) { *(uint64_t *)TO_HOST(state->gp_regs[inst->rs1] + (int64_t)inst->imm) = state->fp_regs[inst->rs2].l; }
static void func_fmadd_d(state_t *state, inst_t *inst) { R4FUNC_D(rs1 * rs2 + rs3); }
static void func_fmsub_d(state_t *state, inst_t *inst) { R4FUNC_D(rs1 * rs2 - rs3); }
static void func_fnmsub_d(state_t *state, inst_t *inst) { R4FUNC_D(-(rs1 * rs2) + rs3); }
static void func_fnmadd_d(state_t *state, inst_t *inst) { R4FUNC_D(-(rs1 * rs2) - rs3); }
static void func_fadd_d(state_t *state, inst_t *inst) { RFUNC_F_D(rs1 + rs2); }
static void func_fsub_d(state_t *state, inst_t *inst) { RFUNC_F_D(rs1 - rs2); }
static void func_fmul_d(state_t *state, inst_t *inst) { RFUNC_F_D(rs1 * rs2); }
static void func_fdiv_d(state_t *state, inst_t *inst) { RFUNC_F_D(rs1 / rs2); }
static void func_fsqrt_d(state_t *state, inst_t *inst) { RFUNC_F_D(sqrt(rs1)); }
static void func_fsgnj_d(state_t *state, inst_t *inst) { printf("fsgn !\n"); }
static void func_fsgnjn_d(state_t *state, inst_t *inst) { printf("fsgn !\n"); }
static void func_fsgnjx_d(state_t *state, inst_t *inst) { printf("fsgn !\n"); }
static void func_fmin_d(state_t *state, inst_t *inst) { RFUNC_F_D(rs1 < rs2 ? rs1 : rs2); }
static void func_fmax_d(state_t *state, inst_t *inst) { RFUNC_F_D(rs1 > rs2 ? rs1 : rs2); }
static void func_fcvt_s_d(state_t *state, inst_t *inst) { state->fp_regs[inst->rd].s = (float)state->fp_regs[inst->rs1].d; }
static void func_fcvt_d_s(state_t *state, inst_t *inst) { state->fp_regs[inst->rd].d = (double)state->fp_regs[inst->rs1].s; }
static void func_feq_d(state_t *state, inst_t *inst) { RFUNC_F_D_L(rs1 == rs2); }
static void func_flt_d(state_t *state, inst_t *inst) { RFUNC_F_D_L(rs1 < rs2); }
static void func_fle_d(state_t *state, inst_t *inst) { RFUNC_F_D_L(rs1 <= rs2); }
static void func_fclass_d(state_t *state, inst_t *inst) { printf("fclass_d !\n"); }
static void func_fcvt_w_d(state_t *state, inst_t *inst) { state->gp_regs[inst->rd] = (int64_t)(int32_t)state->fp_regs[inst->rs1].d; }
static void func_fcvt_wu_d(state_t *state, inst_t *inst) { state->gp_regs[inst->rd] = (int64_t)(int32_t)(uint32_t)state->fp_regs[inst->rs1].d; }
static void func_fcvt_d_w(state_t *state, inst_t *inst) { state->fp_regs[inst->rd].d = (double)(int32_t)state->gp_regs[inst->rs1]; }
static void func_fcvt_d_wu(state_t *state, inst_t *inst) { state->fp_regs[inst->rd].d = (double)(uint32_t)state->gp_regs[inst->rs1]; }
static void func_fcvt_l_d(state_t *state, inst_t *inst) { state->gp_regs[inst->rd] = (int64_t)state->fp_regs[inst->rs1].d; }
static void func_fcvt_lu_d(state_t *state, inst_t *inst) { state->gp_regs[inst->rd] = (uint64_t)state->fp_regs[inst->rs1].d; }
// fmv?
static void func_fmv_x_d(state_t *state, inst_t *inst) { state->gp_regs[inst->rs1] = state->fp_regs[inst->rd].l; }
static void func_fcvt_d_l(state_t *state, inst_t *inst) { state->fp_regs[inst->rd].d = (double)(int64_t)state->gp_regs[inst->rs1]; }
static void func_fcvt_d_lu(state_t *state, inst_t *inst) { state->fp_regs[inst->rd].d = (double)(uint64_t)state->gp_regs[inst->rs1]; }
static void func_fmv_d_x(state_t *state, inst_t *inst) { state->fp_regs[inst->rd].l = state->gp_regs[inst->rs1]; }

func_t *funcs[] = {
    func_lui,
    func_auipc,
    func_jal,
    func_jalr,
    func_beq,
    func_bne,
    func_blt,
    func_bge,
    func_bltu,
    func_bgeu,
    func_lb,
    func_lh,
    func_lw,
    func_lbu,
    func_lhu,
    func_sb,
    func_sh,
    func_sw,
    func_addi,
    func_slti,
    func_sltiu,
    func_xori,
    func_ori,
    func_andi,
    func_slli,
    func_srli,
    func_srai,
    func_add,
    func_sub,
    func_sll,
    func_slt,
    func_sltu,
    func_xor,
    func_srl,
    func_sra,
    func_or,
    func_and,
    func_fence,
    func_fence_i,
    func_ecall,
    func_ebreak,
    func_csrrw,
    func_csrrs,
    func_csrrc,
    func_csrrwi,
    func_csrrsi,
    func_csrrci,
    func_lwu,
    func_ld,
    func_sd,
    func_addiw,
    func_slliw,
    func_srliw,
    func_sraiw,
    func_addw,
    func_subw,
    func_sllw,
    func_srlw,
    func_sraw,
    func_mul,
    func_mulh,
    func_mulhsu,
    func_mulhu,
    func_div,
    func_divu,
    func_rem,
    func_remu,
    func_mulw,
    func_divw,
    func_divuw,
    func_remw,
    func_remuw,
    func_flw,
    func_fsw,
    func_fmadd_s,
    func_fmsub_s,
    func_fnmsub_s,
    func_fnmadd_s,
    func_fadd_s,
    func_fsub_s,
    func_fmul_s,
    func_fdiv_s,
    func_fsqrt_s,
    func_fsgnj_s,
    func_fsgnjn_s,
    func_fsgnjx_s,
    func_fmin_s,
    func_fmax_s,
    func_fcvt_w_s,
    func_fcvt_wu_s,
    func_fmv_x_w,
    func_feq_s,
    func_flt_s,
    func_fle_s,
    func_fclass_s,
    func_fcvt_s_w,
    func_fcvt_s_wu,
    func_fmv_w_x,
    func_fcvt_l_s,
    func_fcvt_lu_s,
    func_fcvt_s_l,
    func_fcvt_s_lu,
    func_fld,
    func_fsd,
    func_fmadd_d,
    func_fmsub_d,
    func_fnmsub_d,
    func_fnmadd_d,
    func_fadd_d,
    func_fsub_d,
    func_fmul_d,
    func_fdiv_d,
    func_fsqrt_d,
    func_fsgnj_d,
    func_fsgnjn_d,
    func_fsgnjx_d,
    func_fmin_d,
    func_fmax_d,
    func_fcvt_s_d,
    func_fcvt_d_s,
    func_feq_d,
    func_flt_d,
    func_fle_d,
    func_fclass_d,
    func_fcvt_w_d,
    func_fcvt_wu_d,
    func_fcvt_d_w,
    func_fcvt_d_wu,
    func_fcvt_l_d,
    func_fcvt_lu_d,
    func_fmv_x_d,
    func_fcvt_d_l,
    func_fcvt_d_lu,
    func_fmv_d_x,
};

#ifdef DEBUG
// int insts_execed = 0;
#endif

void interp_exec_bb(state_t *state)
{
    while (true)
    {
        inst_t inst = {0};
#ifdef DEBUG
        printf("fetch inst from pc : %lx\n", state->pc);
#endif
        uint32_t raw_inst = *(uint32_t *)TO_HOST(state->pc);
#ifdef DEBUG
        printf("fetch inst success, raw_inst : %x\n", raw_inst);
#endif
        inst_decode(&inst, raw_inst);
#ifdef DEBUG
        debug_inst(state, &inst);
        printf("before exec : \n");
#endif
        funcs[inst.type](state, &inst);
        state->gp_regs[zero] = 0;
#ifdef DEBUG
        printf("after exec : \n");
        debug_reg(state);
        // if (state->pc == 0x888000107c0)
        //     MYEXIT("0x888000107c0 arrive");
        // if (++insts_execed >= 410)
        //     MYEXIT("stop for debug");
        printf("\n\n");
#endif

        if (inst.stop)
            break;

        state->pc += inst.rvc ? 2 : 4;
#ifdef DEBUG
        printf("*****************\n");
#endif
    }
}