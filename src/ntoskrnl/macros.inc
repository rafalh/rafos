//macro idt_int seg_selector*,offset*,dpl*,p*
MACRO(idt_int, seg_selector, offset, dpl, p)
  .word (VAL(offset) and HEX(ffff))
  .word VAL(seg_selector)
  .byte 0
  .byte (0b00001110 | (VAL(p) << 7) | (VAL(dpl) << 5))
  .word (VAL(offset) >> 16)
ENDM

//gdt_ldt(seg_limit*,base_address*,type*,system*,dpl*,present*,avl*,_64*,d_b*,granularity*)
MACRO(gdt_ldt, seg_limit, base_address, type, system, dpl, present, avl, _64, d_b, granularity)
  if 1   //VAL(d_b)-16  
    .set _db, 0
  else
    .set _db, 1
  endif
  
  .word (VAL(seg_limit) and HEX(ffff))
  .word (VAL(base_address) and HEX(ffff))
  .byte ((VAL(base_address) >> 16) and HEX(ff))
  .byte (VAL(present) << 7) | (dpl << 5) | (VAL(system) << 4) | type
  .byte (VAL(granularity) << 7) | (_db << 6) | (VAL(_64) << 5) | (VAL(avl) << 4) | ((VAL(seg_limit) >> 16) and 0x0F)
  .byte (VAL(base_address) >> 24)
ENDM

MACRO(ccall1, func, a)
  push VAL(a)
  call VAL(func)
  add esp, 4
ENDM

MACRO(ccall2, func, a, b)
  push VAL(b)
  push VAL(a)
  call VAL(func)
  add esp, 8
ENDM

MACRO(ccall3, func, a, b, c)
  push VAL(c)
  push VAL(b)
  push VAL(a)
  call VAL(func)
  add esp, 12
ENDM

MACRO(ccall4, func, a, b, c, d)
  push VAL(d)
  push VAL(c)
  push VAL(b)
  push VAL(a)
  call VAL(func)
  add esp, 16
ENDM

MACRO(ccall5, func, a, b, c, d, e)
  push VAL(e)
  push VAL(d)
  push VAL(c)
  push VAL(b)
  push VAL(a)
  call VAL(func)
  add esp, 20
ENDM

// data
#define ro 0
#define accessed 1
#define rw 2
#define ro_expand_down 4
#define rw_expand_down 6
// code
#define execute 8
#define execute_ro 10
#define conforming_execute 12
#define conforming_execute_ro 14
