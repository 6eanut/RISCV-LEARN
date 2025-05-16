#ifndef INST_H
#define INST_H

enum inst_type_t
{
    inst_lui, // RVI
    inst_auipc,
    inst_jal,
    inst_jalr,
    inst_beq,
    inst_bne,
    inst_blt,
    inst_bge,
    inst_bltu,
    inst_bgeu,
    inst_lb,
    inst_lh,
    inst_lw,
    inst_lbu,
    inst_lhu,
    inst_sb,
    inst_sh,
    inst_sw,
    inst_addi,
    inst_slti,
    inst_sltiu,
    inst_xori,
    inst_ori,
    inst_andi,
    inst_slli,
    inst_srli,
    inst_srai,
    inst_add,
    inst_sub,
    inst_sll,
    inst_slt,
    inst_sltu,
    inst_xor,
    inst_srl,
    inst_sra,
    inst_or,
    inst_and,
    inst_fence,
    inst_fence_i,
    inst_ecall,
    inst_ebreak,
    inst_csrrw,
    inst_csrrs,
    inst_csrrc,
    inst_csrrwi,
    inst_csrrsi,
    inst_csrrci,
    inst_lwu,
    inst_ld,
    inst_sd,
    inst_addiw,
    inst_slliw,
    inst_srliw,
    inst_sraiw,
    inst_addw,
    inst_subw,
    inst_sllw,
    inst_srlw,
    inst_sraw,
    inst_mul, // RVM
    inst_mulh,
    inst_mulhsu,
    inst_mulhu,
    inst_div,
    inst_divu,
    inst_rem,
    inst_remu,
    inst_mulw,
    inst_divw,
    inst_divuw,
    inst_remw,
    inst_remuw,
    inst_flw, // RVF
    inst_fsw,
    inst_fmadd_s,
    inst_fmsub_s,
    inst_fnmsub_s,
    inst_fnmadd_s,
    inst_fadd_s,
    inst_fsub_s,
    inst_fmul_s,
    inst_fdiv_s,
    inst_fsqrt_s,
    inst_fsgnj_s,
    inst_fsgnjn_s,
    inst_fsgnjx_s,
    inst_fmin_s,
    inst_fmax_s,
    inst_fcvt_w_s,
    inst_fcvt_wu_s,
    inst_fmv_x_w,
    inst_feq_s,
    inst_flt_s,
    inst_fle_s,
    inst_fclass_s,
    inst_fcvt_s_w,
    inst_fcvt_s_wu,
    inst_fmv_w_x,
    inst_fcvt_l_s,
    inst_fcvt_lu_s,
    inst_fcvt_s_l,
    inst_fcvt_s_lu,
    inst_fld, // RVD
    inst_fsd,
    inst_fmadd_d,
    inst_fmsub_d,
    inst_fnmsub_d,
    inst_fnmadd_d,
    inst_fadd_d,
    inst_fsub_d,
    inst_fmul_d,
    inst_fdiv_d,
    inst_fsqrt_d,
    inst_fsgnj_d,
    inst_fsgnjn_d,
    inst_fsgnjx_d,
    inst_fmin_d,
    inst_fmax_d,
    inst_fcvt_s_d,
    inst_fcvt_d_s,
    inst_feq_d,
    inst_flt_d,
    inst_fle_d,
    inst_fclass_d,
    inst_fcvt_w_d,
    inst_fcvt_wu_d,
    inst_fcvt_d_w,
    inst_fcvt_d_wu,
    inst_fcvt_l_d,
    inst_fcvt_lu_d,
    inst_fmv_x_d,
    inst_fcvt_d_l,
    inst_fcvt_d_lu,
    inst_fmv_d_x,
    num_insts,
};

typedef struct
{
    uint8_t rd;
    uint8_t rs1;
    uint8_t rs2;
    int32_t imm;
    bool rvc;
    enum inst_type_t type;
    bool stop;
    uint16_t csr;
    uint8_t rs3;
} inst_t;

#define QUADRANT(raw_inst) (((raw_inst) >> 0) & 0x3)

// RV Standard
#define OPCODE(raw_inst) (((raw_inst) >> 2) & 0x1f)  // 6:2
#define RD(raw_inst) (((raw_inst) >> 7) & 0x1f)      // 11:7
#define FUNCT3(raw_inst) (((raw_inst) >> 12) & 0x7)  // 14:12
#define RS1(raw_inst) (((raw_inst) >> 15) & 0x1f)    // 19:15
#define RS2(raw_inst) (((raw_inst) >> 20) & 0x1f)    // 24:20
#define FUNCT7(raw_inst) (((raw_inst) >> 25) & 0x7f) // 31:25
#define CSR(raw_inst) (((raw_inst) >> 20) & 0xfff)   // 31:20
#define RS3(raw_inst) (((raw_inst) >> 27) & 0x1f)    // 31:27
#define FUNCT2(raw_inst) (((raw_inst) >> 25) & 0x10) // 26:25
#define FUNCT6(raw_inst) (((raw_inst) >> 26) & 0x3f) // 31:26

// RVC
#define CRS2(raw_inst) (((raw_inst) >> 2) & 0x1f)         // 6:2
#define CRS1(raw_inst) (((raw_inst) >> 7) & 0x1f)         // 11:7
#define CRD(raw_inst) (((raw_inst) >> 7) & 0x1f)          // 11:7
#define CFUNCT4(raw_inst) (((raw_inst) >> 12) & 0xf)      // 15:12
#define CFUNCT3(raw_inst) (((raw_inst) >> 13) & 0x7)      // 15:13
#define CRDP_LOW(raw_inst) (((raw_inst) >> 2) & 0x7)      // 4:2
#define CRDP_HIGH(raw_inst) (((raw_inst) >> 7) & 0x7)     // 9:7
#define CRS1P(raw_inst) (((raw_inst) >> 7) & 0x7)         // 9:7
#define CRS2P(raw_inst) (((raw_inst) >> 2) & 0x7)         // 4:2
#define CFUNCT2_LOW(raw_inst) (((raw_inst) >> 5) & 0x3)   // 6:5
#define CFUNCT2_HIGH(raw_inst) (((raw_inst) >> 10) & 0x3) // 11:10
#define CFUNCT6(raw_inst) (((raw_inst) >> 10) & 0x3f)     // 15:10
#define CFUNCT1(raw_inst) (((raw_inst) >> 12) & 0x1)      // 12

void inst_decode(inst_t *, uint32_t);

#endif