#ifndef _SEGMENTS_H_
#define _SEGMENTS_H_

#include <stdint.h>

#pragma pack(push, 1)

struct GDTEntry
{
    uint16_t limit_0_15;       // limit bits 0..15
    uint16_t base_0_15;        // base bits 0..15
    uint8_t base_16_23;        // base bits 16..23
    uint8_t access;            // access flags
    uint8_t flags_limit_16_19; // flags and limit bits 16..19
    uint8_t base_24_31;        // base bits 24..31
};

struct GDTLimitAddress
{
    uint16_t limit;   // length of GTD table
    uint32_t address; // physical address of GDTEntry
};

#pragma pack(pop)

enum GDTFlags
{
    GDT_PAGE_GR = 0x80,
    GDT_32      = 0x40,
};

enum GDTAccess
{
    GDT_PRESENT  = 0x80,
    GDT_EXEC     = 0x08,
    GDT_DC       = 0x04,
    GDT_RW       = 0x02,
    GDT_ACCESSED = 0x01,
};

#define GDT_DPL(n) ((n) << 5)

void InitSegments(void);

#endif /* _SEGMENTS_H_ */
