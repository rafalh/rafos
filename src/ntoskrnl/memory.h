#ifndef _MEMORY_H_
#define _MEMORY_H_

#include "sysdef.h"
#include "string.h"

#define PAGE_SIZE 4096

#define PAGE_PRESENT 0x1
#define PAGE_WRITABLE 0x2
#define PAGE_USER 0x4
#define PAGE_SUPERVISOR 0x0
#define PAGE_ACCESSED 0x20
#define PAGE_WRITTEN 0x40

typedef DWORD PHYSICAL_ADDRESS;
typedef PHYSICAL_ADDRESS **PGDIR;

/* Zajeta pamiec */

typedef struct page_frame_t
{
    unsigned page, count;
} page_frame_t;

typedef struct frames_array_buf_t
{
    unsigned c;
    struct frames_array_buf_t *next;
    page_frame_t frames[511];
} frames_array_buf_t;

#define PAGES_IN_ARRAY_BUF 511

/* Wolna pamiec */

typedef struct frame_list_t
{
    unsigned page, count;
    struct frame_list_t *next, *prev;
} frame_list_t;

typedef struct frames_buf_t
{
    unsigned c;
    struct frames_buf_t *next;
    frame_list_t frames[340];
} frames_buf_t;

#define FRAMES_IN_BUF 340

void *malloc(size_t size);
void free(void *ptr);
void init_memory();

PVOID MmAllocateContiguousMemory(SIZE_T NumberOfBytes, PHYSICAL_ADDRESS HighestAcceptableAddress);
VOID MmFreeContiguousMemory(PVOID BaseAddress);

PHYSICAL_ADDRESS MmGetPhysicalAddress(PVOID BaseAddress);

typedef DWORD MM_SYSTEMSIZE;
#define MmSmallSystem 0
#define MmMediumSystem 1
#define MmLargeSystem 2

MM_SYSTEMSIZE MmQuerySystemSize(VOID);

#endif /* _MEMORY_H_ */
