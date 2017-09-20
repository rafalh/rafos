#ifndef _DEBUG_H_
#define _DEBUG_H_

#include "main.h"

#define ASSERT(cond) do { if(!(cond)) krnl_panic("Assertion failed: " #cond); } while(0)
#define DPRINT(fmt, ...) printf(fmt, ##__VA_ARGS__)
#define TRACE(fmt, ...) DPRINT(fmt, ##__VA_ARGS__)

#endif /* _DEBUG_H_ */
