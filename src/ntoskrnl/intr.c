#include "intr.h"
#include "asm.h"

struct IDTDescr g_IDT[256];

#ifdef __cplusplus
extern "C" {
#endif

void int0(void);
void int1(void);
void int2(void);
void int3(void);
void int4(void);
void int5(void);
void int6(void);
void int7(void);
void int8(void);
void int9(void);
void int10(void);
void int11(void);
void int12(void);
void int13(void);
void int14(void);
void int15(void);
void int16(void);
void int17(void);
void int18(void);
void int19(void);

#ifdef __cplusplus
}
#endif

enum PIC_REGISTERS
{
    PIC_CMD_MASTER  = 0x20,
    PIC_DATA_MASTER = 0x21,
    PIC_CMD_SLAVE   = 0xA0,
    PIC_DATA_SLAVE  = 0xA1,
};

void io_wait()
{
    _out(0xEB, 0);
}

void RemapHardwareInterrupts()
{
    // przekierowanie przerwañ sprzêtowych
    _cli();
    int a = _in(PIC_DATA_MASTER);
    int b = _in(PIC_DATA_SLAVE);
    
    _out(PIC_CMD_MASTER, 0x11);
    io_wait();
    _out(PIC_CMD_SLAVE, 0x11);
    io_wait();
    
    _out(PIC_DATA_MASTER, 0x20);
    io_wait();
    
    _out(PIC_DATA_SLAVE, 0x28);
    io_wait();
    
    _out(PIC_DATA_MASTER, 0x04);
    io_wait();
    
    _out(PIC_DATA_SLAVE, 0x02);
    io_wait();
    
    _out(PIC_DATA_MASTER, 0x01);
    io_wait();
    
    _out(PIC_DATA_SLAVE, 0x01);
    io_wait();
    
    _out(PIC_DATA_MASTER, a);
    _out(PIC_DATA_SLAVE, b);
    io_wait();
    
    int i;
    for(i = 0; i < 1000; ++i)
        io_wait();
        
    _out(PIC_DATA_MASTER, 0xFF);
    _out(PIC_DATA_SLAVE, 0xFF);
}

void InitInterrupts()
{
    RemapHardwareInterrupts();
    
    set_intr_gate(0, int0);
    set_intr_gate(1, int1);
    set_intr_gate(2, int2);
    set_intr_gate(3, int3);
    set_intr_gate(4, int4);
    set_intr_gate(5, int5);
    set_intr_gate(6, int6);
    set_intr_gate(7, int7);
    set_intr_gate(8, int8);
    set_intr_gate(9, int9);
    set_intr_gate(10, int10);
    set_intr_gate(11, int11);
    set_intr_gate(12, int12);
    set_intr_gate(13, int13);
    set_intr_gate(14, int14);
    set_intr_gate(15, int15);
    set_intr_gate(16, int16);
    set_intr_gate(17, int17);
    set_intr_gate(18, int18);
    set_intr_gate(19, int19);
    
    struct IDTLimitAddress idt = {256 * 8 - 1, (uint32_t)g_IDT};
    _lidt(&idt);
    
    _sti(); // enabling IRQ
}
