#ifndef DEBUG_H
#define DEBUG_H

void debug_reg(state_t *);

void debug_inst(state_t *, inst_t *);

void debug_syscall(machine_t *);

#define INST_NAME(index)                                     \
    (index == 0 ? "inst_lui" : index == 1 ? "inst_auipc"     \
                           : index == 2   ? "inst_jal"       \
                           : index == 3   ? "inst_jalr"      \
                           : index == 4   ? "inst_beq"       \
                           : index == 5   ? "inst_bne"       \
                           : index == 6   ? "inst_blt"       \
                           : index == 7   ? "inst_bge"       \
                           : index == 8   ? "inst_bltu"      \
                           : index == 9   ? "inst_bgeu"      \
                           : index == 10  ? "inst_lb"        \
                           : index == 11  ? "inst_lh"        \
                           : index == 12  ? "inst_lw"        \
                           : index == 13  ? "inst_lbu"       \
                           : index == 14  ? "inst_lhu"       \
                           : index == 15  ? "inst_sb"        \
                           : index == 16  ? "inst_sh"        \
                           : index == 17  ? "inst_sw"        \
                           : index == 18  ? "inst_addi"      \
                           : index == 19  ? "inst_slti"      \
                           : index == 20  ? "inst_sltiu"     \
                           : index == 21  ? "inst_xori"      \
                           : index == 22  ? "inst_ori"       \
                           : index == 23  ? "inst_andi"      \
                           : index == 24  ? "inst_slli"      \
                           : index == 25  ? "inst_srli"      \
                           : index == 26  ? "inst_srai"      \
                           : index == 27  ? "inst_add"       \
                           : index == 28  ? "inst_sub"       \
                           : index == 29  ? "inst_sll"       \
                           : index == 30  ? "inst_slt"       \
                           : index == 31  ? "inst_sltu"      \
                           : index == 32  ? "inst_xor"       \
                           : index == 33  ? "inst_srl"       \
                           : index == 34  ? "inst_sra"       \
                           : index == 35  ? "inst_or"        \
                           : index == 36  ? "inst_and"       \
                           : index == 37  ? "inst_fence"     \
                           : index == 38  ? "inst_fence_i"   \
                           : index == 39  ? "inst_ecall"     \
                           : index == 40  ? "inst_ebreak"    \
                           : index == 41  ? "inst_csrrw"     \
                           : index == 42  ? "inst_csrrs"     \
                           : index == 43  ? "inst_csrrc"     \
                           : index == 44  ? "inst_csrrwi"    \
                           : index == 45  ? "inst_csrrsi"    \
                           : index == 46  ? "inst_csrrci"    \
                           : index == 47  ? "inst_lwu"       \
                           : index == 48  ? "inst_ld"        \
                           : index == 49  ? "inst_sd"        \
                           : index == 50  ? "inst_addiw"     \
                           : index == 51  ? "inst_slliw"     \
                           : index == 52  ? "inst_srliw"     \
                           : index == 53  ? "inst_sraiw"     \
                           : index == 54  ? "inst_addw"      \
                           : index == 55  ? "inst_subw"      \
                           : index == 56  ? "inst_sllw"      \
                           : index == 57  ? "inst_srlw"      \
                           : index == 58  ? "inst_sraw"      \
                           : index == 59  ? "inst_mul"       \
                           : index == 60  ? "inst_mulh"      \
                           : index == 61  ? "inst_mulhsu"    \
                           : index == 62  ? "inst_mulhu"     \
                           : index == 63  ? "inst_div"       \
                           : index == 64  ? "inst_divu"      \
                           : index == 65  ? "inst_rem"       \
                           : index == 66  ? "inst_remu"      \
                           : index == 67  ? "inst_mulw"      \
                           : index == 68  ? "inst_divw"      \
                           : index == 69  ? "inst_divuw"     \
                           : index == 70  ? "inst_remw"      \
                           : index == 71  ? "inst_remuw"     \
                           : index == 72  ? "inst_flw"       \
                           : index == 73  ? "inst_fsw"       \
                           : index == 74  ? "inst_fmadd_s"   \
                           : index == 75  ? "inst_fmsub_s"   \
                           : index == 76  ? "inst_fnmsub_s"  \
                           : index == 77  ? "inst_fnmadd_s"  \
                           : index == 78  ? "inst_fadd_s"    \
                           : index == 79  ? "inst_fsub_s"    \
                           : index == 80  ? "inst_fmul_s"    \
                           : index == 81  ? "inst_fdiv_s"    \
                           : index == 82  ? "inst_fsqrt_s"   \
                           : index == 83  ? "inst_fsgnj_s"   \
                           : index == 84  ? "inst_fsgnjn_s"  \
                           : index == 85  ? "inst_fsgnjx_s"  \
                           : index == 86  ? "inst_fmin_s"    \
                           : index == 87  ? "inst_fmax_s"    \
                           : index == 88  ? "inst_fcvt_w_s"  \
                           : index == 89  ? "inst_fcvt_wu_s" \
                           : index == 90  ? "inst_fmv_x_w"   \
                           : index == 91  ? "inst_feq_s"     \
                           : index == 92  ? "inst_flt_s"     \
                           : index == 93  ? "inst_fle_s"     \
                           : index == 94  ? "inst_fclass_s"  \
                           : index == 95  ? "inst_fcvt_s_w"  \
                           : index == 96  ? "inst_fcvt_s_wu" \
                           : index == 97  ? "inst_fmv_w_x"   \
                           : index == 98  ? "inst_fcvt_l_s"  \
                           : index == 99  ? "inst_fcvt_lu_s" \
                           : index == 100 ? "inst_fcvt_s_l"  \
                           : index == 101 ? "inst_fcvt_s_lu" \
                           : index == 102 ? "inst_fld"       \
                           : index == 103 ? "inst_fsd"       \
                           : index == 104 ? "inst_fmadd_d"   \
                           : index == 105 ? "inst_fmsub_d"   \
                           : index == 106 ? "inst_fnmsub_d"  \
                           : index == 107 ? "inst_fnmadd_d"  \
                           : index == 108 ? "inst_fadd_d"    \
                           : index == 109 ? "inst_fsub_d"    \
                           : index == 110 ? "inst_fmul_d"    \
                           : index == 111 ? "inst_fdiv_d"    \
                           : index == 112 ? "inst_fsqrt_d"   \
                           : index == 113 ? "inst_fsgnj_d"   \
                           : index == 114 ? "inst_fsgnjn_d"  \
                           : index == 115 ? "inst_fsgnjx_d"  \
                           : index == 116 ? "inst_fmin_d"    \
                           : index == 117 ? "inst_fmax_d"    \
                           : index == 118 ? "inst_fcvt_s_d"  \
                           : index == 119 ? "inst_fcvt_d_s"  \
                           : index == 120 ? "inst_feq_d"     \
                           : index == 121 ? "inst_flt_d"     \
                           : index == 122 ? "inst_fle_d"     \
                           : index == 123 ? "inst_fclass_d"  \
                           : index == 124 ? "inst_fcvt_w_d"  \
                           : index == 125 ? "inst_fcvt_wu_d" \
                           : index == 126 ? "inst_fcvt_d_w"  \
                           : index == 127 ? "inst_fcvt_d_wu" \
                           : index == 128 ? "inst_fcvt_l_d"  \
                           : index == 129 ? "inst_fcvt_lu_d" \
                           : index == 130 ? "inst_fmv_x_d"   \
                           : index == 131 ? "inst_fcvt_d_l"  \
                           : index == 132 ? "inst_fcvt_d_lu" \
                           : index == 133 ? "inst_fmv_d_x"   \
                                          : "unknown")

#endif