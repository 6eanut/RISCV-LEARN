#include "../include/rvemu.h"

void debug_func(state_t *state, inst_t *inst)
{
    printf("pc          :   %lx\n"
           "type        :   %d\n"
           "rs1         :   %hx\n"
           "rs2         :   %hx\n"
           "rd          :   %hx\n"
           "imm         :   %x\n"
           "rvc         :   %d\n"
           "csr         :   %hx\n"
           "stop        :   %d\n",
           state->pc, inst->type,
           inst->rs1, inst->rs2, inst->rd, inst->imm,
           inst->rvc, inst->csr, inst->stop);
}

func_t *funcs[] = {
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
    debug_func,
};

void interp_exec_bb(state_t *state)
{
    while (true)
    {
        inst_t inst = {0};
        uint32_t raw_inst = *(uint32_t *)state->pc;
        printf("raw_inst        :   %x\n", raw_inst);
        inst_decode(&inst, raw_inst);
        funcs[inst.type](state, &inst);
        state->gp_regs[zero] = 0;

        // if (!inst.stop)
        //     break;

        state->pc += inst.rvc ? 2 : 4;
        printf("*****************\n");
    }
}