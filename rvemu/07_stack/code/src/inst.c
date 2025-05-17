#include "../include/rvemu.h"

// RV Standard
static inst_t inst_utype_read(uint32_t raw_inst)
{
    uint32_t imm31_12 = (raw_inst >> 12) & 0xfffff;
    int32_t imm = imm31_12 << 12;
    imm = (imm << 0) >> 0;
    return (inst_t){
        .rd = RD(raw_inst),
        .imm = imm,
    };
}

static inst_t inst_jtype_read(uint32_t raw_inst)
{
    uint32_t imm19_12 = (raw_inst >> 12) & 0xff;
    uint32_t imm11 = (raw_inst >> 20) & 0x1;
    uint32_t imm10_1 = (raw_inst >> 21) & 0x3ff;
    uint32_t imm20 = (raw_inst >> 31) * 0x1;
    int32_t imm = (int32_t)((imm19_12 << 12) + (imm11 << 11) + (imm10_1 << 1) + (imm20 << 20));
    imm = (imm << 11) >> 11;
    return (inst_t){
        .rd = RD(raw_inst),
        .imm = imm,
    };
}

static inst_t inst_itype_read(uint32_t raw_inst)
{
    uint32_t imm11_0 = (raw_inst >> 20) & 0xfff;
    int32_t imm = imm11_0;
    imm = (imm << 20) >> 20;
    return (inst_t){
        .rd = RD(raw_inst),
        .rs1 = RS1(raw_inst),
        .imm = imm,
    };
}

static inst_t inst_btype_read(uint32_t raw_inst)
{
    uint32_t imm11 = (raw_inst >> 7) & 0x1;
    uint32_t imm4_1 = (raw_inst >> 8) & 0xf;
    uint32_t imm10_5 = (raw_inst >> 25) & 0x3f;
    uint32_t imm12 = (raw_inst >> 31) & 0x1;
    int32_t imm = (imm11 << 11) + (imm4_1 << 1) + (imm10_5 << 5) + (imm12 << 12);
    imm = (imm << 19) >> 19;
    return (inst_t){
        .rs1 = RS1(raw_inst),
        .rs2 = RS2(raw_inst),
        .imm = imm,
        .stop = true,
    };
}

static inst_t inst_stype_read(uint32_t raw_inst)
{
    uint32_t imm4_0 = (raw_inst >> 7) & 0x1f;
    uint32_t imm11_5 = (raw_inst >> 25) & 0x7f;
    int32_t imm = (imm4_0 << 0) + (imm11_5 << 5);
    imm = (imm << 20) >> 20;
    return (inst_t){
        .rs1 = RS1(raw_inst),
        .rs2 = RS2(raw_inst),
        .imm = imm,
    };
}

static inst_t inst_rtype_read(uint32_t raw_inst)
{
    return (inst_t){
        .rd = RD(raw_inst),
        .rs1 = RS1(raw_inst),
        .rs2 = RS2(raw_inst),
    };
}

static inst_t inst_csr_read(uint32_t raw_inst)
{
    return (inst_t){
        .rd = RD(raw_inst),
        .rs1 = RS1(raw_inst),
        .csr = CSR(raw_inst),
    };
}

static inst_t inst_r4type_read(uint32_t raw_inst)
{
    return (inst_t){
        .rd = RD(raw_inst),
        .rs1 = RS1(raw_inst),
        .rs2 = RS2(raw_inst),
        .rs3 = RS3(raw_inst),
    };
}

// RVC
static inst_t inst_ciwtype_read(uint16_t raw_inst)
{
    uint32_t imm5_4 = (raw_inst >> 11) & 0x3;
    uint32_t imm9_6 = (raw_inst >> 7) & 0xf;
    uint32_t imm2 = (raw_inst >> 6) & 0x1;
    uint32_t imm3 = (raw_inst >> 5) & 0x1;
    int32_t imm = (imm5_4 << 4) + (imm9_6 << 6) + (imm2 << 2) + (imm3 << 3);
    return (inst_t){
        .rd = CRDP_LOW(raw_inst) + 8,
        .imm = imm,
        .rvc = true,
    };
}

static inst_t inst_cltype_read1(uint16_t raw_inst)
{
    uint32_t imm5_3 = (raw_inst >> 10) & 0x7;
    uint32_t imm7_6 = (raw_inst >> 5) & 0x3;
    int32_t imm = (imm5_3 << 3) + (imm7_6 << 6);
    return (inst_t){
        .rs1 = CRS1P(raw_inst) + 8,
        .rd = CRDP_LOW(raw_inst) + 8,
        .imm = imm,
        .rvc = true,
    };
}

static inst_t inst_cltype_read2(uint16_t raw_inst)
{
    uint32_t imm5_3 = (raw_inst >> 10) & 0x7;
    uint32_t imm2 = (raw_inst >> 6) & 0x1;
    uint32_t imm6 = (raw_inst >> 5) & 0x1;
    int32_t imm = (imm5_3 << 3) + (imm2 << 2) + (imm6 << 6);
    return (inst_t){
        .rs1 = CRS1P(raw_inst) + 8,
        .rd = CRDP_LOW(raw_inst) + 8,
        .imm = imm,
        .rvc = true,
    };
}

static inst_t inst_cstype_read1(uint16_t raw_inst)
{
    uint32_t imm5_3 = (raw_inst >> 10) & 0x7;
    uint32_t imm7_6 = (raw_inst >> 5) & 0x3;
    int32_t imm = (imm5_3 << 3) + (imm7_6 << 6);
    return (inst_t){
        .rs1 = CRS1P(raw_inst) + 8,
        .rs2 = CRS2P(raw_inst) + 8,
        .imm = imm,
        .rvc = true,
    };
}

static inst_t inst_cstype_read2(uint16_t raw_inst)
{
    uint32_t imm5_3 = (raw_inst >> 10) & 0x7;
    uint32_t imm2 = (raw_inst >> 6) & 0x1;
    uint32_t imm6 = (raw_inst >> 5) & 0x1;
    int32_t imm = (imm5_3 << 3) + (imm2 << 2) + (imm6 << 6);
    return (inst_t){
        .rs1 = CRS1P(raw_inst) + 8,
        .rs2 = CRS2P(raw_inst) + 8,
        .imm = imm,
        .rvc = true,
    };
}

static inst_t inst_cimm540_read(uint16_t raw_inst)
{
    uint32_t imm4_0 = (raw_inst >> 2) & 0x1f;
    uint32_t imm5 = (raw_inst >> 12) & 0x1;
    int32_t imm = (imm4_0 << 0) + (imm5 << 5);
    imm = (imm << 26) >> 26;
    return (inst_t){
        .imm = imm,
        .rvc = true,
    };
}

static inst_t inst_cimm946875_read(uint16_t raw_inst)
{
    uint32_t imm9 = (raw_inst >> 12) & 0x1;
    uint32_t imm4 = (raw_inst >> 6) & 0x1;
    uint32_t imm6 = (raw_inst >> 5) & 0x1;
    uint32_t imm8_7 = (raw_inst >> 3) & 0x3;
    uint32_t imm5 = (raw_inst >> 2) & 0x1;
    int32_t imm = (imm9 << 9) + (imm4 << 4) + (imm6 << 6) + (imm8_7 << 7) + (imm5 << 5);
    imm = (imm << 22) >> 22;
    return (inst_t){
        .imm = imm,
        .rvc = true,
    };
}

static inst_t inst_cimm171612_read(uint16_t raw_inst)
{
    uint32_t imm17 = (raw_inst >> 12) & 0x1;
    uint32_t imm16_12 = (raw_inst >> 2) & 0x1f;
    int32_t imm = (imm17 << 17) + (imm16_12 << 12);
    imm = (imm << 14) >> 14;
    return (inst_t){
        .imm = imm,
        .rvc = true,
    };
}

static inst_t inst_cimm114981067315_read(uint16_t raw_inst)
{
    uint32_t imm11 = (raw_inst >> 12) & 0x1;
    uint32_t imm4 = (raw_inst >> 11) & 0x1;
    uint32_t imm9_8 = (raw_inst >> 9) & 0x3;
    uint32_t imm10 = (raw_inst >> 8) & 0x1;
    uint32_t imm6 = (raw_inst >> 7) & 0x1;
    uint32_t imm7 = (raw_inst >> 6) & 0x1;
    uint32_t imm3_1 = (raw_inst >> 3) & 0x7;
    uint32_t imm5 = (raw_inst >> 2) & 0x11;
    int32_t imm = (imm11 << 11) + (imm4 << 4) + (imm9_8 << 8) + (imm10 << 10) + (imm6 << 6) + (imm7 << 7) + (imm3_1 << 1) + (imm5 << 5);
    imm = (imm << 20) >> 20;
    return (inst_t){
        .imm = imm,
        .rvc = true,
    };
}

static inst_t inst_cimm84376215_read(uint16_t raw_inst)
{
    uint32_t imm8 = (raw_inst >> 12) & 0x1;
    uint32_t imm4_3 = (raw_inst >> 10) & 0x3;
    uint32_t imm7_6 = (raw_inst >> 5) & 0x3;
    uint32_t imm2_1 = (raw_inst >> 3) & 0x3;
    uint32_t imm5 = (raw_inst >> 2) & 0x1;
    int32_t imm = (imm8 << 8) + (imm4_3 << 3) + (imm7_6 << 6) + (imm2_1 << 1) + (imm5 << 5);
    imm = (imm << 23) >> 23;
    return (inst_t){
        .imm = imm,
        .rvc = true,
    };
}

static inst_t inst_cimm54386_read(uint16_t raw_inst)
{
    uint32_t imm5 = (raw_inst >> 12) & 0x1;
    uint32_t imm4_3 = (raw_inst >> 5) & 0x3;
    uint32_t imm8_6 = (raw_inst >> 2) & 0x7;
    int32_t imm = (imm5 << 5) + (imm4_3 << 3) + (imm8_6 << 6);
    return (inst_t){
        .imm = imm,
        .rvc = true,
    };
}

static inst_t inst_cimm54276_read(uint16_t raw_inst)
{
    uint32_t imm5 = (raw_inst >> 12) & 0x1;
    uint32_t imm4_2 = (raw_inst >> 4) & 0x7;
    uint32_t imm7_6 = (raw_inst >> 2) & 0x3;
    int32_t imm = (imm5 << 5) + (imm4_2 << 2) + (imm7_6 << 6);
    return (inst_t){
        .imm = imm,
        .rvc = true,
    };
}

static inst_t inst_cimm5386_read(uint16_t raw_inst)
{
    uint32_t imm5_3 = (raw_inst >> 10) & 0x7;
    uint32_t imm8_6 = (raw_inst >> 7) & 0x7;
    int32_t imm = (imm5_3 << 3) + (imm8_6 << 6);
    return (inst_t){
        .imm = imm,
        .rvc = true,
    };
}

static inst_t inst_cimm5276_read(uint16_t raw_inst)
{
    uint32_t imm5_2 = (raw_inst >> 9) & 0xf;
    uint32_t imm7_6 = (raw_inst >> 7) & 0x3;
    int32_t imm = (imm5_2 << 2) + (imm7_6 << 6);
    return (inst_t){
        .imm = imm,
        .rvc = true,
    };
}

void inst_decode(inst_t *inst, uint32_t raw_inst)
{
    uint32_t quadrant = QUADRANT(raw_inst);
    switch (quadrant)
    {
    // convert raw_inst to inst
    case 0x0: // rvc
    {
        uint32_t funct3 = CFUNCT3(raw_inst);
        inst->rvc = true;
        switch (funct3)
        {
        case 0x0: // c.addi4spn : addi rd, x2, uimm
        {         // 000 uimm[5:4|9:6|2|3] rd' 00
            *inst = inst_ciwtype_read(raw_inst);
            inst->type = inst_addi;
            inst->rs1 = sp;
            if (inst->imm == 0)
                MYEXIT("Illegal instruction");
            return;
        }
        case 0x1:
        {
            *inst = inst_cltype_read1(raw_inst);
            inst->type = inst_fld;
            return;
        }
        case 0x2:
        {
            *inst = inst_cltype_read2(raw_inst);
            inst->type = inst_lw;
            return;
        }
        case 0x3:
        {
            *inst = inst_cltype_read1(raw_inst);
            inst->type = inst_ld;
            return;
        }
        case 0x5:
        {
            *inst = inst_cstype_read1(raw_inst);
            inst->type = inst_fsd;
            return;
        }
        case 0x6:
        {
            *inst = inst_cstype_read2(raw_inst);
            inst->type = inst_sw;
            return;
        }
        case 0x7:
        {
            *inst = inst_cstype_read2(raw_inst);
            inst->type = inst_fsw;
            return;
        }
        default:
            MYEXIT("inst_decode fail");
        }
    }
    case 0x1: // rvc
    {
        uint32_t funct3 = CFUNCT3(raw_inst);
        inst->rvc = true;
        switch (funct3)
        {
        case 0x0: // c.addi
        {
            *inst = inst_cimm540_read(raw_inst);
            assert(inst->imm != 0);
            inst->type = inst_addi;
            inst->rd = CRD(raw_inst);
            inst->rs1 = CRS1(raw_inst);
            return;
        }
        case 0x1: // c.addiw
        {
            *inst = inst_cimm540_read(raw_inst);
            inst->type = inst_addiw;
            inst->rd = CRD(raw_inst);
            assert(inst->rd != 0);
            inst->rs1 = CRS1(raw_inst);
            return;
        }
        case 0x2: // c.li
        {
            *inst = inst_cimm540_read(raw_inst);
            inst->type = inst_addi;
            inst->rd = CRD(raw_inst);
            inst->rs1 = zero;
            return;
        }
        case 0x3:
        {
            uint32_t rd = CRD(raw_inst);
            if (rd == 2) // c.addi16s[]
            {
                *inst = inst_cimm946875_read(raw_inst);
                assert(inst->imm != 0);
                inst->type = inst_addi;
                inst->rs1 = rd;
                inst->rd = rd;
                return;
            }
            else // c.lui
            {
                *inst = inst_cimm171612_read(raw_inst);
                assert(inst->imm != 0);
                inst->type = inst_lui;
                inst->rd = rd;
                return;
            }
        }
        case 0x4:
        {
            uint32_t funct2_high = CFUNCT2_HIGH(raw_inst);
            switch (funct2_high)
            {
            case 0x0: // c.srli
            {
                *inst = inst_cimm540_read(raw_inst);
                inst->type = inst_srli;
                inst->rs1 = CRS1P(raw_inst) + 8;
                inst->rd = CRDP_HIGH(raw_inst) + 8;
                return;
            }
            case 0x1: // c.srai
            {
                *inst = inst_cimm540_read(raw_inst);
                inst->type = inst_srai;
                inst->rs1 = CRS1P(raw_inst) + 8;
                inst->rd = CRDP_HIGH(raw_inst) + 8;
                return;
            }
            case 0x2: // c.andi
            {
                *inst = inst_cimm540_read(raw_inst);
                inst->type = inst_addi;
                inst->rs1 = CRS1P(raw_inst) + 8;
                inst->rd = CRDP_HIGH(raw_inst) + 8;
                return;
            }
            case 0x3:
            {
                uint32_t funct2_low = CFUNCT2_LOW(raw_inst);
                switch (funct2_low)
                {
                case 0x0: // c.sub
                {
                    inst->rs1 = CRS1P(raw_inst) + 8;
                    inst->rd = CRDP_HIGH(raw_inst) + 8;
                    inst->rs2 = CRS2P(raw_inst) + 8;
                    inst->type = inst_sub;
                    return;
                }
                case 0x1: // c.xor
                {
                    inst->rs1 = CRS1P(raw_inst) + 8;
                    inst->rd = CRDP_HIGH(raw_inst) + 8;
                    inst->rs2 = CRS2P(raw_inst) + 8;
                    inst->type = inst_xor;
                    return;
                }
                case 0x2: // c.or
                {
                    inst->rs1 = CRS1P(raw_inst) + 8;
                    inst->rd = CRDP_HIGH(raw_inst) + 8;
                    inst->rs2 = CRS2P(raw_inst) + 8;
                    inst->type = inst_or;
                    return;
                }
                case 0x3: // c.and
                {
                    inst->rs1 = CRS1P(raw_inst) + 8;
                    inst->rd = CRDP_HIGH(raw_inst) + 8;
                    inst->rs2 = CRS2P(raw_inst) + 8;
                    inst->type = inst_and;
                    return;
                }
                default:
                    MYEXIT("inst_decode fail");
                }
            }
            default:
                MYEXIT("inst_decode fail");
            }
        }
        case 0x5: // c.j
        {
            *inst = inst_cimm114981067315_read(raw_inst);
            inst->type = inst_jal;
            inst->rd = zero;
            inst->stop = true;
            return;
        }
        case 0x6: // c.beqz
        {
            *inst = inst_cimm84376215_read(raw_inst);
            inst->type = inst_beq;
            inst->rs1 = CRS1P(raw_inst) + 8;
            inst->rs2 = zero;
            inst->stop = true;
            return;
        }
        case 0x7: // c.bnez
        {
            *inst = inst_cimm84376215_read(raw_inst);
            inst->rs1 = CRS1P(raw_inst) + 8;
            inst->rs2 = zero;
            inst->type = inst_bne;
            inst->stop = true;
            return;
        }
        default:
            MYEXIT("inst_decode fail");
        }
    }
    case 0x2: // rvc
    {
        uint32_t funct3 = CFUNCT3(raw_inst);
        inst->rvc = true;
        switch (funct3)
        {
        case 0x0: // c.slli
        {
            *inst = inst_cimm540_read(raw_inst);
            inst->type = inst_slli;
            inst->rs1 = CRS1(raw_inst);
            inst->rd = CRD(raw_inst);
            return;
        }
        case 0x1: // c.fldsp
        {
            *inst = inst_cimm54386_read(raw_inst);
            inst->rd = CRD(raw_inst);
            inst->rs1 = sp;
            inst->type = inst_fld;
            return;
        }
        case 0x2: // c.lwsp
        {
            *inst = inst_cimm54276_read(raw_inst);
            inst->rd = CRD(raw_inst);
            inst->rs1 = sp;
            inst->type = inst_lw;
            return;
        }
        case 0x3: // c.ldsp
        {
            *inst = inst_cimm54386_read(raw_inst);
            inst->rd = CRD(raw_inst);
            inst->rs1 = sp;
            inst->type = inst_ld;
            return;
        }
        case 0x4:
        {
            uint32_t funct1 = CFUNCT1(raw_inst);
            if (funct1)
            {
                uint32_t rs1 = CRS1(raw_inst);
                uint32_t rs2 = CRS2(raw_inst);
                if (rs1 == 0 && rs2 == 0) // c.ebreak
                {
                    inst->rs1 = rs1;
                    inst->rs2 = rs2;
                    inst->type = inst_ebreak;
                    return;
                }
                if (rs1 != 0 && rs2 == 0) // c.jalr
                {
                    inst->rs1 = rs1;
                    inst->rs2 = rs2;
                    inst->rd = ra;
                    inst->type = inst_jalr;
                    inst->stop = true;
                    return;
                }
                if (rs1 != 0 && rs2 != 0) // c.add
                {
                    inst->rs1 = rs1;
                    inst->rs2 = rs2;
                    inst->rd = rs1;
                    inst->type = inst_add;
                    return;
                }
                MYEXIT("inst_decode fail");
            }
            else
            {
                uint32_t rs1 = CRS1(raw_inst);
                uint32_t rs2 = CRS2(raw_inst);
                if (rs1 != 0 && rs2 == 0) // c.jr
                {
                    inst->rs1 = rs1;
                    inst->rs2 = rs2;
                    inst->rd = zero;
                    inst->type = inst_jalr;
                    inst->stop = true;
                    return;
                }
                if (rs1 != 0 && rs2 != 0) // c.mv
                {
                    inst->rs1 = zero;
                    inst->rs2 = rs2;
                    inst->rd = rs1;
                    inst->type = inst_add;
                    return;
                }
                MYEXIT("inst_decode fail");
            }
        }
        case 0x5: // c.fsdsp
        {
            *inst = inst_cimm5386_read(raw_inst);
            inst->rs2 = CRS2(raw_inst);
            inst->rs1 = sp;
            inst->type = inst_fsd;
            return;
        }
        case 0x6: // c.swsp
        {
            *inst = inst_cimm5276_read(raw_inst);
            inst->rs2 = CRS2(raw_inst);
            inst->rs1 = sp;
            inst->type = inst_sw;
            return;
        }
        case 0x7: // c.sdsp
        {
            *inst = inst_cimm5386_read(raw_inst);
            inst->rs2 = CRS2(raw_inst);
            inst->rs1 = sp;
            inst->type = inst_sd;
            return;
        }
        default:
            MYEXIT("inst_decode fail");
        }
    }
    case 0x3: // standard
    {
        uint32_t opcode = OPCODE(raw_inst);
        switch (opcode)
        {
        case 0xd: // lui
        {
            *inst = inst_utype_read(raw_inst);
            inst->type = inst_lui;
            return;
        }
        case 0x5: // auipc
        {
            *inst = inst_utype_read(raw_inst);
            inst->type = inst_auipc;
            return;
        }
        case 0x1b: // jal
        {
            *inst = inst_jtype_read(raw_inst);
            inst->type = inst_jal;
            inst->stop = true;
            return;
        }
        case 0x19: // j-type
        {
            uint32_t funct3 = FUNCT3(raw_inst);
            *inst = inst_itype_read(raw_inst);
            switch (funct3)
            {
            case 0x0: // jalr
            {
                inst->type = inst_jalr;
                inst->stop = true;
                return;
            }
            default:
                MYEXIT("inst_decode fail");
            }
        }
        case 0x18: // b-type
        {
            uint32_t funct3 = FUNCT3(raw_inst);
            *inst = inst_btype_read(raw_inst);
            switch (funct3)
            {
            case 0x0: // beq
            {
                inst->type = inst_beq;
                return;
            }
            case 0x1: // bne
            {
                inst->type = inst_bne;
                return;
            }
            case 0x4: // blt
            {
                inst->type = inst_blt;
                return;
            }
            case 0x5: // bge
            {
                inst->type = inst_bge;
                return;
            }
            case 0x6: // bltu
            {
                inst->type = inst_bltu;
                return;
            }
            case 0x7: // bgeu
            {
                inst->type = inst_bgeu;
                return;
            }
            default:
                MYEXIT("inst_decode fail");
            }
        }
        case 0x0: // i-type
        {
            uint32_t funct3 = FUNCT3(raw_inst);
            *inst = inst_itype_read(raw_inst);
            switch (funct3)
            {
            case 0x0: // lb
            {
                inst->type = inst_lb;
                return;
            }
            case 0x1: // lh
            {
                inst->type = inst_lh;
                return;
            }
            case 0x2: // lw
            {
                inst->type = inst_lw;
                return;
            }
            case 0x4: // lbu
            {
                inst->type = inst_lbu;
                return;
            }
            case 0x5: // lhu
            {
                inst->type = inst_lhu;
                return;
            }
            case 0x6: // lwu
            {
                inst->type = inst_lwu;
                return;
            }
            case 0x3: // ld
            {
                inst->type = inst_ld;
                return;
            }
            default:
                MYEXIT("inst_decode fail");
            }
        }
        case 0x8: // s-type
        {
            uint32_t funct3 = FUNCT3(raw_inst);
            *inst = inst_stype_read(raw_inst);
            switch (funct3)
            {
            case 0x0: // sb
            {
                inst->type = inst_sb;
                return;
            }
            case 0x1: // sh
            {
                inst->type = inst_sh;
                return;
            }
            case 0x2: // sw
            {
                inst->type = inst_sw;
                return;
            }
            case 0x3: // sd
            {
                inst->type = inst_sd;
                return;
            }
            default:
                MYEXIT("inst_decode fail");
            }
        }
        case 0x4: // i-type
        {
            uint32_t funct3 = FUNCT3(raw_inst);
            *inst = inst_itype_read(raw_inst);
            switch (funct3)
            {
            case 0x0: // addi
            {
                inst->type = inst_addi;
                return;
            }
            case 0x2: // slti
            {
                inst->type = inst_slti;
                return;
            }
            case 0x3: // sltiu
            {
                inst->type = inst_sltiu;
                return;
            }
            case 0x4: // xori
            {
                inst->type = inst_xori;
                return;
            }
            case 0x6: // ori
            {
                inst->type = inst_ori;
                return;
            }
            case 0x7: // andi
            {
                inst->type = inst_andi;
                return;
            }
            case 0x1:
            {
                uint32_t funct6 = FUNCT6(raw_inst);
                switch (funct6)
                {
                case 0x0: // slli
                {
                    inst->type = inst_slli;
                    return;
                }
                default:
                    MYEXIT("inst_decode fail");
                }
            }
            case 0x5:
            {
                uint32_t funct6 = FUNCT6(raw_inst);
                switch (funct6)
                {
                case 0x0: // srli
                {
                    inst->type = inst_srli;
                    return;
                }
                case 0x10: // srai
                {
                    inst->type = inst_srai;
                    return;
                }
                default:
                    MYEXIT("inst_decode fail");
                }
            }
            default:
                MYEXIT("inst_decode fail");
            }
        }
        case 0xc: // r-type
        {
            uint32_t funct3 = FUNCT3(raw_inst);
            *inst = inst_rtype_read(raw_inst);
            switch (funct3)
            {
            case 0x0:
            {
                uint32_t funct7 = FUNCT7(raw_inst);
                switch (funct7)
                {
                case 0x0: // add
                {
                    inst->type = inst_add;
                    return;
                }
                case 0x20: // sub
                {
                    inst->type = inst_sub;
                    return;
                }
                case 0x1: // mul
                {
                    inst->type = inst_mul;
                    return;
                }
                default:
                    MYEXIT("inst_decode fail");
                }
            }
            case 0x1:
            {
                uint32_t funct7 = FUNCT7(raw_inst);
                switch (funct7)
                {
                case 0x0: // sll
                {
                    inst->type = inst_sll;
                    return;
                }
                case 0x1: // mulh
                {
                    inst->type = inst_mulh;
                    return;
                }
                default:
                    MYEXIT("inst_decode fail");
                }
            }
            case 0x2:
            {
                uint32_t funct7 = FUNCT7(raw_inst);
                switch (funct7)
                {
                case 0x0: // slt
                {
                    inst->type = inst_slt;
                    return;
                }
                case 0x1: // mulhsu
                {
                    inst->type = inst_mulhsu;
                    return;
                }
                default:
                    MYEXIT("inst_decode fail");
                }
            }
            case 0x3:
            {
                uint32_t funct7 = FUNCT7(raw_inst);
                switch (funct7)
                {
                case 0x0: // sltu
                {
                    inst->type = inst_sltu;
                    return;
                }
                case 0x1: // mulhu
                {
                    inst->type = inst_mulhu;
                    return;
                }
                default:
                    MYEXIT("inst_decode fail");
                }
            }
            case 0x4:
            {
                uint32_t funct7 = FUNCT7(raw_inst);
                switch (funct7)
                {
                case 0x0: // xor
                {
                    inst->type = inst_xor;
                    return;
                }
                case 0x1: // div
                {
                    inst->type = inst_div;
                    return;
                }
                default:
                    MYEXIT("inst_decode fail");
                }
            }
            case 0x5:
            {
                uint32_t funct7 = FUNCT7(raw_inst);
                switch (funct7)
                {
                case 0x0: // srl
                {
                    inst->type = inst_srl;
                    return;
                }
                case 0x20: // sra
                {
                    inst->type = inst_sra;
                    return;
                }
                case 0x1: // divu
                {
                    inst->type = inst_divu;
                    return;
                }
                default:
                    MYEXIT("inst_decode fail");
                }
            }
            case 0x6:
            {
                uint32_t funct7 = FUNCT7(raw_inst);
                switch (funct7)
                {
                case 0x0: // or
                {
                    inst->type = inst_or;
                    return;
                }
                case 0x1: // rem
                {
                    inst->type = inst_rem;
                    return;
                }
                default:
                    MYEXIT("inst_decode fail");
                }
            }
            case 0x7:
            {
                uint32_t funct7 = FUNCT7(raw_inst);
                switch (funct7)
                {
                case 0x0: // and
                {
                    inst->type = inst_and;
                    return;
                }
                case 0x1: // remu
                {
                    inst->type = inst_remu;
                    return;
                }
                default:
                    MYEXIT("inst_decode fail");
                }
            }
            default:
                MYEXIT("inst_decode fail");
            }
        }
        case 0x3: // i-type
        {
            uint32_t funct3 = FUNCT3(raw_inst);
            *inst = inst_itype_read(raw_inst);
            switch (funct3)
            {
            case 0x0: // fence
            {
                inst->type = inst_fence;
                return;
            }
            case 0x1: // fence.i
            {
                inst->type = inst_fence_i;
                return;
            }
            default:
                MYEXIT("inst_decode fail");
            }
        }
        case 0x1c: // i-type
        {
            uint32_t funct3 = FUNCT3(raw_inst);
            *inst = inst_itype_read(raw_inst);
            switch (funct3)
            {
            case 0x0: // ecall ebreak
            {
                if ((raw_inst & 0x100000) == 0)
                {
                    inst->type = inst_ecall;
                    inst->stop = true;
                }
                else
                {
                    inst->type = inst_ebreak;
                }
                return;
            }
            case 0x1: // csrrw
            {
                *inst = inst_csr_read(raw_inst);
                inst->type = inst_csrrw;
                return;
            }
            case 0x2: // csrrs
            {
                *inst = inst_csr_read(raw_inst);
                inst->type = inst_csrrs;
                return;
            }
            case 0x3: // csrrc
            {
                *inst = inst_csr_read(raw_inst);
                inst->type = inst_csrrc;
                return;
            }
            case 0x5: // csrrwi
            {
                *inst = inst_csr_read(raw_inst);
                inst->type = inst_csrrwi;
                return;
            }
            case 0x6: // csrrsi
            {
                *inst = inst_csr_read(raw_inst);
                inst->type = inst_csrrsi;
                return;
            }
            case 0x7: // csrrci
            {
                *inst = inst_csr_read(raw_inst);
                inst->type = inst_csrrci;
                return;
            }
            default:
                MYEXIT("inst_decode fail");
            }
        }
        case 0x6: // i-type
        {
            uint32_t funct3 = FUNCT3(raw_inst);
            *inst = inst_itype_read(raw_inst);
            switch (funct3)
            {
            case 0x0: // addiw
            {
                inst->type = inst_addiw;
                return;
            }
            case 0x1: // slliw
            {
                inst->type = inst_slliw;
                return;
            }
            case 0x5: // srliw sraiw
            {
                uint32_t funct7 = FUNCT7(raw_inst);
                switch (funct7)
                {
                case 0x0: // srliw
                {
                    inst->type = inst_srliw;
                    return;
                }
                case 0x20: // sraiw
                {
                    inst->type = inst_sraiw;
                    return;
                }
                default:
                    MYEXIT("inst_decode fail");
                }
            }
            default:
                MYEXIT("inst_decode fail");
            }
        }
        case 0xe: // r-type
        {
            uint32_t funct3 = FUNCT3(raw_inst);
            *inst = inst_rtype_read(raw_inst);
            switch (funct3)
            {
            case 0x0:
            {
                uint32_t funct7 = FUNCT7(raw_inst);
                switch (funct7)
                {
                case 0x0: // addw
                {
                    inst->type = inst_addw;
                    return;
                }
                case 0x20: // subw
                {
                    inst->type = inst_subw;
                    return;
                }
                case 0x1: // mulw
                {
                    inst->type = inst_mulw;
                    return;
                }
                default:
                    MYEXIT("inst_decode fail");
                }
            }
            case 0x1:
            {
                uint32_t funct7 = FUNCT7(raw_inst);
                switch (funct7)
                {
                case 0x0: // sllw
                {
                    inst->type = inst_sllw;
                    return;
                }
                default:
                    MYEXIT("inst_decode fail");
                }
            }
            case 0x5:
            {
                uint32_t funct7 = FUNCT7(raw_inst);
                switch (funct7)
                {
                case 0x0: // srlw
                {
                    inst->type = inst_srlw;
                    return;
                }
                case 0x20: // sraw
                {
                    inst->type = inst_sraw;
                    return;
                }
                case 0x1: // divuw
                {
                    inst->type = inst_divuw;
                    return;
                }
                default:
                    MYEXIT("inst_decode fail");
                }
            }
            case 0x4:
            {
                uint32_t funct7 = FUNCT7(raw_inst);
                switch (funct7)
                {
                case 0x1: // divw
                {
                    inst->type = inst_divw;
                    return;
                }
                default:
                    MYEXIT("inst_decode fail");
                }
            }
            case 0x6:
            {
                uint32_t funct7 = FUNCT7(raw_inst);
                switch (funct7)
                {
                case 0x1: // remw
                {
                    inst->type = inst_remw;
                    return;
                }
                default:
                    MYEXIT("inst_decode fail");
                }
            }
            case 0x7:
            {
                uint32_t funct7 = FUNCT7(raw_inst);
                switch (funct7)
                {
                case 0x1: // remuw
                {
                    inst->type = inst_remuw;
                    return;
                }
                default:
                    MYEXIT("inst_decode fail");
                }
            }
            default:
                MYEXIT("inst_decode fail");
            }
        }
        case 0x1: // i-type
        {
            uint32_t funct3 = FUNCT3(raw_inst);
            *inst = inst_itype_read(raw_inst);
            switch (funct3)
            {
            case 0x2:
            {
                inst->type = inst_flw;
                return;
            }
            case 0x3:
            {
                inst->type = inst_fld;
                return;
            }
            default:
                MYEXIT("inst_decode fail");
            }
        }
        case 0x9: // s-type
        {
            uint32_t funct3 = FUNCT3(raw_inst);
            *inst = inst_stype_read(raw_inst);
            switch (funct3)
            {
            case 0x2:
            {
                inst->type = inst_fsw;
                return;
            }
            case 0x3:
            {
                inst->type = inst_fsd;
                return;
            }
            default:
                MYEXIT("inst_decode fail");
            }
        }
        case 0x10:
        {
            uint32_t funct2 = FUNCT2(raw_inst);
            *inst = inst_r4type_read(raw_inst);
            switch (funct2)
            {
            case 0x0:
            {
                inst->type = inst_fmadd_s;
                return;
            }
            case 0x1:
            {
                inst->type = inst_fmadd_d;
                return;
            }
            default:
                MYEXIT("inst_decode fail");
            }
        }
        case 0x11:
        {
            uint32_t funct2 = FUNCT2(raw_inst);
            *inst = inst_r4type_read(raw_inst);
            switch (funct2)
            {
            case 0x0:
            {
                inst->type = inst_fmsub_s;
                return;
            }
            case 0x1:
            {
                inst->type = inst_fmsub_d;
                return;
            }
            default:
                MYEXIT("inst_decode fail");
            }
        }
        case 0x12:
        {
            uint32_t funct2 = FUNCT2(raw_inst);
            *inst = inst_r4type_read(raw_inst);
            switch (funct2)
            {
            case 0x0:
            {
                inst->type = inst_fnmsub_s;
                return;
            }
            case 0x1:
            {
                inst->type = inst_fnmsub_d;
                return;
            }
            default:
                MYEXIT("inst_decode fail");
            }
        }
        case 0x13:
        {
            uint32_t funct2 = FUNCT2(raw_inst);
            *inst = inst_r4type_read(raw_inst);
            switch (funct2)
            {
            case 0x0:
            {
                inst->type = inst_fnmadd_s;
                return;
            }
            case 0x1:
            {
                inst->type = inst_fnmadd_d;
                return;
            }
            default:
                MYEXIT("inst_decode fail");
            }
        }
        case 0x14: // r-type
        {
            uint32_t funct7 = FUNCT7(raw_inst);
            *inst = inst_rtype_read(raw_inst);
            switch (funct7)
            {
            case 0x0:
            {
                inst->type = inst_fadd_s;
                return;
            }
            case 0x1:
            {
                inst->type = inst_fadd_d;
                return;
            }
            case 0x4:
            {
                inst->type = inst_fsub_s;
                return;
            }
            case 0x5:
            {
                inst->type = inst_fsub_d;
                return;
            }
            case 0x8:
            {
                inst->type = inst_fmul_s;
                return;
            }
            case 0x9:
            {
                inst->type = inst_fmul_d;
                return;
            }
            case 0xc:
            {
                inst->type = inst_fdiv_s;
                return;
            }
            case 0xd:
            {
                inst->type = inst_fdiv_d;
                return;
            }
            case 0x2c:
            {
                inst->type = inst_fsqrt_s;
                return;
            }
            case 0x2d:
            {
                inst->type = inst_fsqrt_d;
                return;
            }
            case 0x10:
            {
                uint32_t funct3 = FUNCT3(raw_inst);
                switch (funct3)
                {
                case 0x0:
                {
                    inst->type = inst_fsgnj_s;
                    return;
                }
                case 0x1:
                {
                    inst->type = inst_fsgnjn_s;
                    return;
                }
                case 0x2:
                {
                    inst->type = inst_fsgnjx_s;
                    return;
                }
                default:
                    MYEXIT("inst_decode fail");
                }
            }
            case 0x11:
            {
                uint32_t funct3 = FUNCT3(raw_inst);
                switch (funct3)
                {
                case 0x0:
                {
                    inst->type = inst_fsgnj_d;
                    return;
                }
                case 0x1:
                {
                    inst->type = inst_fsgnjn_d;
                    return;
                }
                case 0x2:
                {
                    inst->type = inst_fsgnjx_d;
                    return;
                }
                default:
                    MYEXIT("inst_decode fail");
                }
            }
            case 0x14:
            {
                uint32_t funct3 = FUNCT3(raw_inst);
                switch (funct3)
                {
                case 0x0:
                {
                    inst->type = inst_fmin_s;
                    return;
                }
                case 0x1:
                {
                    inst->type = inst_fmax_s;
                    return;
                }
                default:
                    MYEXIT("inst_decode fail");
                }
            }
            case 0x15:
            {
                uint32_t funct3 = FUNCT3(raw_inst);
                switch (funct3)
                {
                case 0x0:
                {
                    inst->type = inst_fmin_d;
                    return;
                }
                case 0x1:
                {
                    inst->type = inst_fmax_d;
                    return;
                }
                default:
                    MYEXIT("inst_decode fail");
                }
            }
            case 0x40:
            {
                inst->type = inst_fcvt_s_d;
                return;
            }
            case 0x41:
            {
                inst->type = inst_fcvt_d_s;
                return;
            }
            case 0x51:
            {
                uint32_t funct3 = FUNCT3(raw_inst);
                switch (funct3)
                {
                case 0x0:
                {
                    inst->type = inst_fle_d;
                    return;
                }
                case 0x1:
                {
                    inst->type = inst_flt_d;
                    return;
                }
                case 0x2:
                {
                    inst->type = inst_feq_d;
                    return;
                }
                default:
                    MYEXIT("inst_decode fail");
                }
            }
            case 0x71:
            {
                uint32_t funct3 = FUNCT3(raw_inst);
                switch (funct3)
                {
                case 0x0:
                {
                    inst->type = inst_fmv_x_d;
                    return;
                }
                case 0x1:
                {
                    inst->type = inst_fclass_d;
                    return;
                }
                default:
                    MYEXIT("inst_decode fail");
                }
            }
            case 0x60:
            {
                switch (inst->rs2)
                {
                case 0x0:
                {
                    inst->type = inst_fcvt_w_s;
                    return;
                }
                case 0x1:
                {
                    inst->type = inst_fcvt_wu_s;
                    return;
                }
                case 0x2:
                {
                    inst->type = inst_fcvt_l_s;
                    return;
                }
                case 0x3:
                {
                    inst->type = inst_fcvt_lu_s;
                    return;
                }
                default:
                    MYEXIT("inst_decode fail");
                }
            }
            case 0x61:
            {
                switch (inst->rs2)
                {
                case 0x0:
                {
                    inst->type = inst_fcvt_w_d;
                    return;
                }
                case 0x1:
                {
                    inst->type = inst_fcvt_wu_d;
                    return;
                }
                case 0x2:
                {
                    inst->type = inst_fcvt_l_d;
                    return;
                }
                case 0x3:
                {
                    inst->type = inst_fcvt_lu_d;
                    return;
                }
                default:
                    MYEXIT("inst_decode fail");
                }
            }
            case 0x70:
            {
                uint32_t funct3 = FUNCT3(raw_inst);
                switch (funct3)
                {
                case 0x0:
                {
                    inst->type = inst_fmv_x_w;
                    return;
                }
                case 0x1:
                {
                    inst->type = inst_fclass_s;
                    return;
                }
                default:
                    MYEXIT("inst_decode fail");
                }
            }
            case 0x50:
            {
                uint32_t funct3 = FUNCT3(raw_inst);
                switch (funct3)
                {
                case 0x0:
                {
                    inst->type = inst_fle_s;
                    return;
                }
                case 0x1:
                {
                    inst->type = inst_flt_s;
                    return;
                }
                case 0x2:
                {
                    inst->type = inst_feq_s;
                    return;
                }
                default:
                    MYEXIT("inst_decode fail");
                }
            }
            case 0x68:
            {
                switch (inst->rs2)
                {
                case 0x0:
                {
                    inst->type = inst_fcvt_s_w;
                    return;
                }
                case 0x1:
                {
                    inst->type = inst_fcvt_s_wu;
                    return;
                }
                case 0x2:
                {
                    inst->type = inst_fcvt_s_l;
                    return;
                }
                case 0x3:
                {
                    inst->type = inst_fcvt_s_lu;
                    return;
                }
                default:
                    MYEXIT("inst_decode fail");
                }
            }
            case 0x69:
            {
                switch (inst->rs2)
                {
                case 0x0:
                {
                    inst->type = inst_fcvt_d_w;
                    return;
                }
                case 0x1:
                {
                    inst->type = inst_fcvt_d_wu;
                    return;
                }
                case 0x2:
                {
                    inst->type = inst_fcvt_d_l;
                    return;
                }
                case 0x3:
                {
                    inst->type = inst_fcvt_d_lu;
                    return;
                }
                default:
                    MYEXIT("inst_decode fail");
                }
            }
            case 0x78:
            {
                inst->type = inst_fmv_w_x;
                return;
            }
            case 0x79:
            {
                inst->type = inst_fmv_d_x;
                return;
            }
            default:
                MYEXIT("inst_decode fail");
            }
        }
        default:
            MYEXIT("inst_decode fail");
        }
    }
    default:
        MYEXIT("inst_decode fail");
    }
}