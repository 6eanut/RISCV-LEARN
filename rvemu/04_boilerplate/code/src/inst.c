#include "../include/rvemu.h"

#define QUADRANT(raw_inst) (raw_inst & 0x3)

void inst_decode(inst_t *inst, uint32_t raw_inst)
{
    uint32_t quadrant = QUADRANT(raw_inst);
    switch (quadrant)
    {
    // convert raw_inst to inst
    case 0x0:
        MYEXIT("quadrant : 0x0");
        break;
    case 0x1:
        MYEXIT("quadrant : 0x1");
        break;
    case 0x2:
        MYEXIT("quadrant : 0x2");
        break;
    case 0x3:
        MYEXIT("quadrant : 0x3");
        break;
    default:
        break;
    }
}