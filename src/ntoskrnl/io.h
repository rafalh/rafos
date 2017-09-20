#ifndef _IO_H_
#define _IO_H_

#include "asm.h"

//#define outportb(port, val) _out(port, val)
//#define inportb(port, ret) ret = _in(port);

#define outportb(port, val) __asm__ __volatile__("outb %%al,%%dx"::"a"(val),"d"(port))
#define inportb(port, ret) __asm__ __volatile__("inb %%dx,%%al":"=a"(ret):"d"(port))

#endif /* _IO_H_ */
