##include "macros.inc"

.intel_syntax noprefix

.text

.global _irq0
_irq0:
  push gs
  push fs
  push es
  push ds
  pusha
  ;mov ax, 0x10
  ;mov ds, ax
  ;mov es, ax
.extern _timer_irq
  call _timer_irq

  mov al, 0x20 # EOI
  out 0x20, al

  popa
  pop ds
  pop es
  pop fs
  pop gs

  iretd
