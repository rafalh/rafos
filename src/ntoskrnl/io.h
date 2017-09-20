#ifndef _IO_H_
#define _IO_H_

#include "asm.h"

#define outportb(port, val) _out(port, val)
#define inportb(port, ret) ret = _in(port);

#endif /* _IO_H_ */
