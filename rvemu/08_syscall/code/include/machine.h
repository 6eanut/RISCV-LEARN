#ifndef MACHINE_H
#define MACHINE_H

// host program : rvemu
// guest program : rvprogram

// rvemu reads rvprogram's ELF Header and Program Header Table
// ELF Header -> entry, stored in mmu
// Program Header Table -> .text .data, stored in rvemu's memory(host alloc)
// ATTENTION : heap in rvprogram has to be emulated, so just follows host alloc

#define STACK_SIZE 32 * 1024 * 1024

enum exit_reason_t
{
    ecall,
    direct_branch,
    indirect_branch,
    num_reasons,
};

typedef struct
{
    uint64_t pc;
    gp_reg_t gp_regs[num_gp_regs];
    fp_reg_t fp_regs[num_fp_regs];
    enum exit_reason_t exit_reason;
} state_t;

typedef struct
{
    state_t state;
    mmu_t mmu;
} machine_t;

void machine_load_program(machine_t *, char *);
void machine_step(machine_t *);
void machine_mmu_init(machine_t *, uint64_t, char *[]);
uint64_t machine_get_gp_reg(machine_t *, uint32_t);
void machine_set_gp_reg(machine_t *, uint32_t, uint64_t);

#endif
