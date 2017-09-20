#ifndef _INTR_H_
#define _INTR_H_

#include <stdint.h>

#pragma pack(push, 1)

struct IDTDescr
{
    uint16_t offset_1; // offset bits 0..15
    uint16_t selector; // a code segment selector in GDT or LDT
    uint8_t zero;      // unused, set to 0
    uint8_t type_attr; // type and attributes, see below
    uint16_t offset_2; // offset bits 16..31
};

struct IDTLimitAddress
{
    uint16_t limit;
    uint32_t address;
};

#pragma pack(pop)

extern struct IDTDescr g_IDT[256];

#define _set_gate(n, type, addr) {\
    g_IDT[n].offset_1 = (((uint32_t)(addr))&0xffff); \
    g_IDT[n].selector = 0x8; \
    g_IDT[n].zero = 0x0; \
    g_IDT[n].type_attr = (type); \
    g_IDT[n].offset_2 = (((uint32_t)(addr))>>16); }

#define IDT_P      0x80
#define IDT_DPL(n) ((n) << 5)
#define IDT_S      0x10
#define IDT_32     0x08
#define IDT_TASK_GATE  0x05
#define IDT_INTR_GATE  0x0E
#define IDT_TRAP_GATE  0x0F

#define set_intr_gate(n, addr) _set_gate(n, IDT_P|IDT_INTR_GATE, addr)
#define set_trap_gate(n, addr) _set_gate(n, IDT_P|IDT_TRAP_GATE, addr)
#define set_system_gate(n, addr) _set_gate(n, IDT_P|IDT_TASK_GATE|IDT_DPL(3), addr)

void InitInterrupts(void);

#endif /* _INTR_H_ */
