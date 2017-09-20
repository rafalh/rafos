#ifndef _KERNEL_H_
#define _KERNEL_H_

#include "sysdef.h"

#define APIENTRY

void reboot(void);
void krnl_panic(const char *msg);
void mem_dump(const void *ptr, unsigned len);

#endif /* _KERNEL_H_ */
