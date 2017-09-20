#ifndef _ASM_H_
#define _ASM_H_

#include <stdint.h>

#ifndef FORCEINLINE
#define FORCEINLINE static
#endif

FORCEINLINE void _sti()
{
    __asm__ __volatile__("sti");
}

FORCEINLINE void _cli()
{
    __asm__ __volatile__("cli");
}

FORCEINLINE void _out(unsigned short port, unsigned char val)
{
    __asm__ __volatile__("outb %%al,%%dx"::"a"(val), "d"(port));
}

FORCEINLINE unsigned char _in(unsigned short port)
{
    unsigned char ret;
    __asm__ __volatile__("inb %%dx,%%al":"=a"(ret):"d"(port));
    return ret;
}

FORCEINLINE void _lgdt(void *addr)
{
    __asm__ __volatile__("lgdt %0"::"m"(*(int*)addr));
}

FORCEINLINE void _lidt(void *addr)
{
    __asm__ __volatile__("lidt %0"::"m"(*(int*)addr));
}

FORCEINLINE void _nop()
{
    __asm__ __volatile__("nop");
}

FORCEINLINE void _hlt()
{
    __asm__ __volatile__("hlt");
}

#endif // _ASM_H_
