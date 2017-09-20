#include "irq.h"
#include "io.h"

unsigned cached_irq_mask = 0xffff;

#define __byte(x,y)    (((unsigned char *)&(y))[x])
#define cached_21      (__byte(0, cached_irq_mask))
#define cached_A1      (__byte(1, cached_irq_mask))

void disable_irq(unsigned int irq)
{
    cached_irq_mask |= 1 << irq;
    if(irq & 8) outportb(0xA1, cached_A1);
    else outportb(0x21, cached_21);
}

void enable_irq(unsigned int irq)
{
    cached_irq_mask &= ~(1 << irq);
    if(irq & 8) outportb(0xA1, cached_A1);
    else outportb(0x21, cached_21);
}
