#ifndef _THREADS_H_
#define _THREADS_H_

#include "memory.h"

typedef struct
{
    DWORD
    backlink,
    esp0,
    ss0,
    esp1,
    ss1,
    esp2,
    ss2,
    cr3,
    eip,
    eflags,
    eax,
    ecx,
    edx,
    ebx,
    esp,
    ebp,
    esi,
    edi,
    es,
    cs,
    ss,
    ds,
    fs,
    gs,
    ldt,
    bmoffset;
} tss_t;

typedef struct thread_t
{
    unsigned pid;
    
} thread_t;


#endif /* _THREADS_H_ */
