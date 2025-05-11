#ifndef INTERP_H
#define INTERP_H

typedef void(func_t)(state_t *, inst_t *);

void interp_exec_bb(state_t *);
#endif