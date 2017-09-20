.intel_syntax noprefix
.text

.global _start
_start:
  # Init stack
  lea esp, [stack_end]

.extern _kmain
  push eax
  push ebx
  cli
  call _kmain   # run kmain() from C code
  cli
  hlt
loop: jmp loop

.bss

stack_begin:
# 64 KB
.space 64*1024
stack_end:
