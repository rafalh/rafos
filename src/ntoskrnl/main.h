#ifndef _KERNEL_H_
#define _KERNEL_H_

#include "sysdef.h"

#define APIENTRY

void reboot();
void ProcessCmdLine();
void krnl_panic(const char *msg);
void krnl_start(void);
void mem_dump(const void *ptr, unsigned len);

#endif /* _KERNEL_H_ */
