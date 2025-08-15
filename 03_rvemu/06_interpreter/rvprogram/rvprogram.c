#include <stdint.h>
#include <stdio.h>
int main()
{
    uint32_t imm_z = 0x00001234;
    uint32_t imm_f = 0xffff1234;
    printf("imm_z                       :   %x\n"
           "imm_f                       :   %x\n",
           imm_z, imm_f);

    int64_t imm_z_k = (int64_t)imm_z;
    int64_t imm_f_k = (int64_t)imm_f;
    printf("imm_z_k                     :   %lx\n"
           "imm_f_k                     :   %lx\n",
           imm_z_k, imm_f_k);

    uint64_t rs1 = 0xffffffffffffffff;
    printf("imm_z_k & rs1               :   %lx\n"
           "imm_f_k & rs1               :   %lx\n",
           imm_z_k & rs1, imm_f_k & rs1);
    printf("(uint64_t)imm_z_k & rs1     :   %lx\n"
           "(uint64_t)imm_f_k & rs1     :   %lx\n",
           (uint64_t)imm_z_k & rs1, (uint64_t)imm_f_k & rs1);

    int64_t imm = -2048;                  // 0xFFFFFFFFFFFFF800
    uint64_t rs2 = 0x00000000FFFFFFFF;    // 仅低 32 位有效
    printf("%lx\n", rs2 & imm);           // 错误：0x00000000FFFFF800（符号扩展污染）
    printf("%lx\n", rs2 & (uint64_t)imm); // 正确：0x000000000000F800（无符号截断）
}