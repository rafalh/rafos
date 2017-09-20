#include "asm.inc"

.code

PUBLIC _irq6
_irq6:
  push gs
  push fs
  push es
  push ds
  pusha
  //mov ax, 0x10
  //mov ds, ax
  //mov es, ax
     
EXTERN _floppy_irq
  call _floppy_irq
  
  mov al, 0x20 //EOI
  out 0x20, al
  
  popa
  pop ds
  pop es
  pop fs
  pop gs
  iretd