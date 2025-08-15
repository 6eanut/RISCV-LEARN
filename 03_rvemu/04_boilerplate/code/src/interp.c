#include "../include/rvemu.h"

func_t *funcs[] = {};

void interp_exec_bb(state_t *state)
{
    inst_t inst = {0};
    while (true)
    {
        uint32_t raw_inst = *(uint32_t *)state->pc;
        inst_decode(&inst, raw_inst);
        funcs[inst.type](state, &inst);
        state->gp_regs[zero] = 0;

        if (!inst.goon)
            break;

        state->pc += inst.rvc ? 2 : 4;
    }
}