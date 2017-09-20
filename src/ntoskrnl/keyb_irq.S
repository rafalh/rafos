#include "asm.inc"

.code

PUBLIC _irq1
_irq1:
  push gs
  push fs
  push es
  push ds
  pusha
  //mov ax, 0x10
  //mov ds, ax
  //mov es, ax

EXTERN _keyb_irq
  call _keyb_irq

  mov al, 0x20 //EOI
  out 0x20, al

  popa
  pop ds
  pop es
  pop fs
  pop gs
  iretd
