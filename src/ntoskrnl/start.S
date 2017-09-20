#include "asm.inc"
#include "macros.inc"

.code32

#define BASE_OF_SECTION 0x00400000
PUBLIC _begin
_begin:

EXTERN ___text_begin__
EXTERN ___data_end__
EXTERN ___bss_end__

.align 4

# MULTIBOOT_HEADER_MAGIC  equ 0x1BADB002
# MULTIBOOT_PAGE_ALIGN    equ 0x00000001
# MULTIBOOT_MEMORY_INFO   equ 0x00000002
# MULTIBOOT_AOUT_KLUDGE   equ 0x00010000

# MULTIBOOT_FLAGS equ MULTIBOOT_PAGE_ALIGN|MULTIBOOT_MEMORY_INFO|MULTIBOOT_AOUT_KLUDGE

# mboot:
#  dd MULTIBOOT_HEADER_MAGIC	; Sygnatura
#  dd MULTIBOOT_FLAGS ; Flagi dla bootloadera
#  dd -(MULTIBOOT_HEADER_MAGIC+MULTIBOOT_FLAGS) ; suma kontrolna nag³ówka
#  dd mboot ; Pozycja nag³ówka w pliku
#  dd ___text_begin__
#  dd ___data_end__
#  dd ___bss_end__
#  dd _start


.equ MULTIBOOT_HEADER_MAGIC, 0x1BADB002
.equ MULTIBOOT_PAGE_ALIGN,   0x00000001
.equ MULTIBOOT_MEMORY_INFO,  0x00000002
.equ MULTIBOOT_AOUT_KLUDGE,  0x00010000

.equ MULTIBOOT_FLAGS,  MULTIBOOT_PAGE_ALIGN|MULTIBOOT_MEMORY_INFO|MULTIBOOT_AOUT_KLUDGE


.section .mboot, "x"
mboot:

# #  .long HEX(1BADB002)	// Sygnatura
# #  .long HEX(10001) // Flagi dla bootloadera
# #  .long -(HEX(1BADB002)+HEX(10001)) // suma kontrolna nag³ówka
# #  .long mboot // Pozycja nag³ówka w pliku
# #  .long ___text_begin__
# #  .long ___data_end__
# #  .long ___bss_end__
# #  .long _start

 .long MULTIBOOT_HEADER_MAGIC # Sygnatura
 .long MULTIBOOT_FLAGS # Flagi dla bootloadera
 .long -(MULTIBOOT_HEADER_MAGIC+MULTIBOOT_FLAGS) # suma kontrolna naglowka
 .long mboot # Pozycja naglowka w pliku
 .long ___text_begin__
 .long ___data_end__
 .long ___bss_end__
 .long _start

.text

PUBLIC _start
_start:
  # Init stack
  mov esp, 0x000A0000

EXTERN _kmain
  push eax
  push ebx
  cli
  call _kmain   # run kmain() from C code
  cli
  hlt
loop: jmp loop
