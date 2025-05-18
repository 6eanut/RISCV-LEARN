#ifndef INTERP_H
#define INTERP_H

typedef void(func_t)(state_t *, inst_t *);

#define RFUNC(expr)                           \
    uint64_t rs1 = state->gp_regs[inst->rs1]; \
    uint64_t rs2 = state->gp_regs[inst->rs2]; \
    state->gp_regs[inst->rd] = (expr);

#define IFUNC(expr)                           \
    int64_t imm = (int64_t)inst->imm;         \
    uint64_t rs1 = state->gp_regs[inst->rs1]; \
    state->gp_regs[inst->rd] = (expr);

#define UFUNC(expr)                   \
    int64_t imm = (int64_t)inst->imm; \
    state->gp_regs[inst->rd] = (expr);

#define BFUNC(expr)                           \
    state->exit_reason = direct_branch;       \
    uint64_t rs1 = state->gp_regs[inst->rs1]; \
    uint64_t rs2 = state->gp_regs[inst->rs2]; \
    int64_t imm = (int64_t)inst->imm;         \
    if ((expr))                               \
    {                                         \
        state->pc += imm;                     \
        inst->stop = true;                    \
    }

#define RFUNC_F_D(expr)                                               \
    double rs1 = state->fp_regs[inst->rs1].d;                         \
    __attribute__((unused)) double rs2 = state->fp_regs[inst->rs2].d; \
    state->fp_regs[inst->rd].d = (expr);

#define RFUNC_F_S(expr)                                              \
    float rs1 = state->fp_regs[inst->rs1].s;                         \
    __attribute__((unused)) float rs2 = state->fp_regs[inst->rs2].s; \
    state->fp_regs[inst->rd].s = (expr);

#define RFUNC_F_D_L(expr)                     \
    double rs1 = state->fp_regs[inst->rs1].d; \
    double rs2 = state->fp_regs[inst->rs2].d; \
    state->gp_regs[inst->rd] = (expr);

#define RFUNC_F_S_L(expr)                    \
    float rs1 = state->fp_regs[inst->rs1].s; \
    float rs2 = state->fp_regs[inst->rs2].s; \
    state->gp_regs[inst->rd] = (expr);

#define R4FUNC_D(expr)                        \
    double rs1 = state->fp_regs[inst->rs1].d; \
    double rs2 = state->fp_regs[inst->rs2].d; \
    double rs3 = state->fp_regs[inst->rs3].d; \
    state->fp_regs[inst->rd].d = (expr);

#define R4FUNC_S(expr)                       \
    float rs1 = state->fp_regs[inst->rs1].s; \
    float rs2 = state->fp_regs[inst->rs2].s; \
    float rs3 = state->fp_regs[inst->rs3].s; \
    state->fp_regs[inst->rd].s = (float)(expr);

void interp_exec_bb(state_t *);
#endif