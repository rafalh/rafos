#ifndef _ASM_H_
#define _ASM_H_

#include <stdint.h>

#ifndef INLINE
#define INLINE static __inline__
#endif

INLINE void _sti()
{
    __asm__ __volatile__ ("sti");
}

INLINE void _cli()
{
    __asm__ __volatile__ ("cli");
}

INLINE void _out(unsigned short port, unsigned char val)
{
    __asm__ __volatile__ ("outb %%al,%%dx"::"a"(val), "d"(port));
}

INLINE unsigned char _in(unsigned short port)
{
    unsigned char ret;
    __asm__ __volatile__ ("inb %%dx,%%al":"=a"(ret):"d"(port));
    return ret;
}

INLINE void _lgdt(void *addr)
{
    __asm__ __volatile__ ("lgdt %0"::"m"(*(int*)addr));
}

INLINE void _lidt(void *addr)
{
    __asm__ __volatile__ ("lidt %0"::"m"(*(int*)addr));
}

INLINE void _nop()
{
    __asm__ __volatile__ ("nop");
}

INLINE void _hlt()
{
    __asm__ __volatile__ ("hlt");
}

#endif // _ASM_H_
