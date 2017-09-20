#include "segments.h"
#include "asm.h"
#include "debug.h"

#define MAX_SEGMENTS 3

struct GDTEntry g_GDT[MAX_SEGMENTS] = {};

void SetSegment(unsigned i, uint32_t base, uint32_t limit, uint8_t access, uint8_t flags)
{
    struct GDTEntry *pEntry = &g_GDT[i];
    pEntry->base_0_15 = base & 0xFFFF;
    pEntry->limit_0_15 = limit & 0xFFFF;
    pEntry->base_24_31 = (base >> 24) & 0xFF;
    pEntry->flags_limit_16_19 = flags | (limit >> 16);
    pEntry->access = access | 0x10;
    pEntry->base_16_23 = (base >> 16) & 0xFF;
    ASSERT(i < MAX_SEGMENTS);
}

void InitSegments()
{
    SetSegment(0, 0, 0, 0, 0); // NULL segment
    SetSegment(1, 0, 0xFFFFFFFF, GDT_PRESENT | GDT_EXEC | GDT_RW | GDT_DPL(0), GDT_PAGE_GR | GDT_32);
    SetSegment(2, 0, 0xFFFFFFFF, GDT_PRESENT | GDT_RW | GDT_DPL(0), GDT_PAGE_GR | GDT_32);
    
    struct GDTLimitAddress gdt = {sizeof(g_GDT) - 1, (uint32_t)g_GDT};
    _lgdt(&gdt);
    __asm__ __volatile__("ljmp $0x8,$cs_updated\r\ncs_updated:");
}
