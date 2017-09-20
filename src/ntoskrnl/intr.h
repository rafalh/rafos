#ifndef _INTR_H_
#define	_INTR_H_

struct desc_t
{
	unsigned short a, b, c, d;
};
extern struct desc_t idt[256];

#define _set_gate(gate_addr, type, addr) {\
	((unsigned short*)(gate_addr))[0] = (((unsigned)(addr))&0xffff); \
	((unsigned short*)(gate_addr))[1] = 0x8; \
	((unsigned short*)(gate_addr))[2] = (type); \
	((unsigned short*)(gate_addr))[3] = (((unsigned)(addr))>>16); }

#define IDT_P 0x8000
#define IDT_32 0x800

#define set_intr_gate(n, addr) _set_gate(&idt[n], IDT_32|IDT_P|0x600, addr)
#define set_trap_gate(n, addr) _set_gate(&idt[n], IDT_32|IDT_P|0x700, addr)
#define set_system_gate(n, addr) _set_gate(&idt[n], 3<<13|IDT_P|0x500, addr)

#endif /* _INTR_H_ */
