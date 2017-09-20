#include "exceptions.h"
#include "conio.h"
#include "main.h"
#include "stdio.h"

static const char *lpIntStr[] =
{
    "Deviding by 0.",
    "",
    "NMI.",
    "Breakpoint.",
    "Overflow.",
    "BOUND instruction break.",
    "Undefined opcode.",
    "No FPU.",
    "Double Fault.",
    "FPU Segment Overrun.",
    "Invalid TSS.",
    "Segment not present.",
    "Stack segment fault.",
    "General Protection Fault.",
    "Page fault.",
    "",
    "Float operation fault.",
    "Alignmen check error.",
    "Machine Check.",
    "SSE/SSE2/SSE3 operation fault."
};

void exception_proc(unsigned n, unsigned eflags, unsigned ds, unsigned es, unsigned fs, unsigned gs, unsigned edi, unsigned esi, unsigned ebp, unsigned esp, unsigned ebx, unsigned edx, unsigned ecx, unsigned eax, unsigned error_code, unsigned addr)
{
    //*(char*)(0xb8000) = 'e';
    //*(char*)(0xb8001) = '\7';
    unsigned i;
    
    textbackground(1);
    textcolor(15);
    clrscr();
    
    printf("Exception %d! %s\nError code: 0x%x\nAddress: 0x%08x\n\nRegisters:\n", n, lpIntStr[n], error_code, addr);
    printf("DS:  0x%08x\tES:  0x%08x\tFS:  0x%08x\tGS:  0x%08x\n", ds, es, fs, gs);
    printf("EDI: 0x%08x\tESI: 0x%08x\tEBP: 0x%08x\tESP: 0x%08x\n", edi, esi, ebp, esp);
    printf("EAX: 0x%08x\tEBX: 0x%08x\tECX: 0x%08x\tEDX: 0x%08x\n", eax, ebx, ecx, edx);
    printf("EFLAGS: 0x%08x\n\nStack (hex):\n", eflags);
    for(i = 0; (esp + i * sizeof(unsigned)) < 0x0A0000 && i < 64; ++i)
        printf("%08x%s", ((unsigned*)esp)[i], ((i & 7) == 7) ? "\n" : " ");
    if((esp + i * sizeof(unsigned)) < 0x0A0000)
        printf("...");
    krnl_panic("");
}
