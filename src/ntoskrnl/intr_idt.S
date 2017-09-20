.intel_syntax noprefix

.text

.extern _exception_proc
exception_asm:
  pusha
  push gs
  push fs
  push es
  push ds
  pushfd
  push dword [exception_nr]
  call _exception_proc
  add esp, 4 # args
  add esp, 4 # popfd
  pop ds
  pop es
  pop fs
  pop gs
  popa
  add esp, 4 # pop error nr
  iretd

.global _int0
.global _int1
.global _int2
.global _int3
.global _int4
.global _int5
.global _int6
.global _int7
.global _int8
.global _int9
.global _int10
.global _int11
.global _int12
.global _int13
.global _int14
.global _int15
.global _int16
.global _int17
.global _int18
.global _int19

_int0:
  mov dword ptr [exception_nr], 0
  push 0
  jmp exception_asm
_int1:
  mov dword ptr  [exception_nr], 1
  push 0
  jmp exception_asm
_int2:
  mov dword ptr [exception_nr], 2
  push 0
  jmp exception_asm
_int3:
  mov dword ptr [exception_nr], 3
  push 0
  jmp exception_asm
_int4:
  mov dword ptr [exception_nr], 4
  push 0
  jmp exception_asm
_int5:
  mov dword ptr [exception_nr], 5
  push 0
  jmp exception_asm
_int6:
  mov dword ptr [exception_nr], 6
  push 0
  jmp exception_asm
_int7:
  mov dword ptr [exception_nr], 7
  push 0
  jmp exception_asm
_int8:
  mov dword ptr [exception_nr], 8
  jmp exception_asm
_int9:
  mov dword ptr [exception_nr], 9
  push 0
  jmp exception_asm
_int10:
  mov dword ptr [exception_nr], 10
  jmp exception_asm
_int11:
  mov dword ptr [exception_nr], 11
  jmp exception_asm
_int12:
  mov dword ptr [exception_nr], 12
  jmp exception_asm
_int13:
  mov dword ptr [exception_nr], 13
  jmp exception_asm
_int14:
  mov dword ptr [exception_nr], 14
  jmp exception_asm
_int15:
  mov dword ptr [exception_nr], 15
  push 0
  jmp exception_asm
_int16:
  mov dword ptr [exception_nr], 16
  push 0
  jmp exception_asm
_int17:
  mov dword ptr [exception_nr], 17
  jmp exception_asm
_int18:
  mov dword ptr [exception_nr], 18
  push 0
  jmp exception_asm
_int19:
  mov dword ptr [exception_nr], 19
  push 0
  jmp exception_asm
  
_empty_irq:
  push eax
  mov al, 0x20 # EOI
  out 0x20, al
  pop eax
  iretd

exception_nr:
 .int 0
