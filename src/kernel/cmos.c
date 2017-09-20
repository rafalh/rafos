#include "cmos.h"
#include "asm.h"

enum CMOS_REGISTER
{
    CMOS_SELECT_REG = 0x70,
    CMOS_READ_REG   = 0x71,
};

unsigned char get_cmos_byte(unsigned char i)
{
    _out(CMOS_SELECT_REG, i);
    _nop();
    _nop();
    _nop();
    return _in(CMOS_READ_REG);
}
