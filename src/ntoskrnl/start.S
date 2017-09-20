#include "asm.inc"
#include "macros.inc"

.code32

#define BASE_OF_SECTION 0x00400000
PUBLIC _begin
_begin:

EXTERN ___text_begin__
EXTERN ___data_end__
EXTERN ___bss_end__

ALIGN 4
mboot:
 .long HEX(1BADB002)	// Sygnatura
 .long HEX(10001) // Flagi dla bootloadera
 .long -HEX(1BADB002)-HEX(10001) // suma kontrolna nag³ówka
 .long mboot // Pozycja nag³ówka w pliku
 .long ___text_begin__
 .long ___data_end__
 .long ___bss_end__
 .long _start

PUBLIC _start
_start:
  cli // wylaczenie przerwan

  // ustaw poprawne segmenty
  mov ax, HEX(10)
  mov ss, ax
  mov ds, ax
  mov fs, ax
  mov es, ax
  mov gs, ax
  mov esp, HEX(000A0000) // stos

  lgdt [gdt_descriptor]
  jmp HEX(8):.1
.1:

  lidt [idt_descriptor]

// wyczysc ekran
  cld
  mov edi, HEX(B8000)
  mov eax, HEX(07200720)
  mov ecx, (80*25)/2
  rep stosd

// przekierowanie przerwañ sprzêtowych
  cli
  in al, HEX(21)
  mov ah, al
  in al, HEX(A1)
  mov cx, ax

  mov al, HEX(11)
  out HEX(20), al
  out HEX(EB), al
  out HEX(A0), al
  out HEX(EB), al

  mov al, HEX(20) // offset dla master PIC
  out 0x21, al
  out 0xEB, al

  mov al, HEX(28) // offset dla slave PIC
  out HEX(A1), al
  out HEX(EB), al

  mov al, HEX(04)
  out HEX(21), al
  out HEX(EB), al
  shr al, 1 ; AL=2
  out HEX(A1), al
  out HEX(EB), al
  shr al, 1; AL=1
  out HEX(21), al
  out HEX(EB), al
  out HEX(A1), al
  out HEX(EB), al

  mov ax, cx
  out HEX(A1), al
  mov al, ah
  out HEX(21), al
  mov ecx, HEX(1000)
  cld

.picl1:
  out HEX(EB), al
  loop .picl1

  cli
  mov al, 255
  out HEX(a1), al
  out HEX(21), al

  sti // w³¹czenie IRQ

EXTERN(_kmain)
  call _kmain   // uruchamiamy kernel
  cli
  hlt
//loop: jmp loop

// wyj¹tek
EXTERN(_exception_proc)
exception_asm:
  pusha
  push gs
  push fs
  push es
  push ds
  pushfd
  push dword ptr [exception_nr]
  call _exception_proc
  add esp, 4 // exception_nr
  add esp, 4 // popfd
  pop ds
  pop es
  pop fs
  pop gs
  popa
  add esp, 4 // pop error nr
  iretd

PUBLIC _get_cmos_byte
_get_cmos_byte: //(unsigned i)
  pushfd
  cli
  xor eax, eax
  mov al, [esp+8]
  out 0x70, al
  nop
  nop
  nop
  in al, HEX(71)
  popfd
  ret

PUBLIC _open_dma_channel
_open_dma_channel: // (unsigned channel, unsigned address, unsigned count, unsigned mode)

  pushfd
  cli
  push eax
  push ebx     //arg1 = [esp+16]

  mov eax, [esp+16]   // channel
  mov bl, al          // BL = channel
  or al, HEX(4)
  out HEX(0a), al        // mask DMA channel
  mov al, HEX(ff)
  out HEX(0c), al        // reset flip-flop
  mov eax, [esp+20]   // address
  out HEX(04), al        // address (low byte)
  mov al, ah
  out HEX(04), al        // address (high byte)
  shr eax, 16
  out HEX(81), al        // external page register
  mov al, 0xff
  out HEX(0c), al        // reset flip-flop (again!!!)
  mov eax, [esp+24]   // count
  out HEX(05), al        // count (low byte)
  mov al, ah
  out HEX(05), al        // count(high byte)
  mov eax, [esp+28]   // mode
  or al, bl
  out HEX(0b), al        // set mode
  mov al, bl
  out HEX(0a), al        // unmask DMA channel

  pop ebx
  pop eax
  popfd
  ret

int0:
  mov dword ptr [exception_nr], 0
  push 0
  jmp exception_asm
int1:
  mov dword ptr [exception_nr], 1
  push 0
  jmp exception_asm
int2:
  mov dword ptr [exception_nr], 2
  push 0
  jmp exception_asm
int3:
  mov dword ptr [exception_nr], 3
  push 0
  jmp exception_asm
int4:
  mov dword ptr [exception_nr], 4
  push 0
  jmp exception_asm
int5:
  mov dword ptr [exception_nr], 5
  push 0
  jmp exception_asm
int6:
  mov dword ptr [exception_nr], 6
  push 0
  jmp exception_asm
int7:
  mov dword ptr [exception_nr], 7
  push 0
  jmp exception_asm
int8:
  mov dword ptr [exception_nr], 8
  jmp exception_asm
int9:
  mov dword ptr [exception_nr], 9
  push 0
  jmp exception_asm
int10:
  mov dword ptr [exception_nr], 10
  jmp exception_asm
int11:
  mov dword ptr [exception_nr], 11
  jmp exception_asm
int12:
  mov dword ptr [exception_nr], 12
  jmp exception_asm
int13:
  mov dword ptr [exception_nr], 13
  jmp exception_asm
int14:
  mov dword ptr [exception_nr], 14
  jmp exception_asm
int15:
  mov dword ptr [exception_nr], 15
  push 0
  jmp exception_asm
int16:
  mov dword ptr [exception_nr], 16
  push 0
  jmp exception_asm
int17:
  mov dword ptr [exception_nr], 17
  jmp exception_asm
int18:
  mov dword ptr [exception_nr], 18
  push 0
  jmp exception_asm
int19:
  mov dword ptr [exception_nr], 19
  push 0
  jmp exception_asm

empty_irq:
  push eax
  mov al, HEX(20) // EOI
  out HEX(20), al
  pop eax
  iretd

.data

gdt:
  .double 0 // null segment
  gdt_ldt HEX(ffff), 0, execute_ro, 1, 0, 1, 1, 0, 32, 1
  gdt_ldt HEX(ffff), 0, rw, 1, 0, 1, 1, 0, 32, 1
gdt_end:

gdt_descriptor:
  .word gdt_end - gdt - 1
  .long gdt

PUBLIC _idt
_idt:
// idt_int seg_selector*, offset*, dpl*, p*
idt_int HEX(8), (BASE_OF_SECTION + int0 - _begin), 0, 1
idt_int HEX(8), (BASE_OF_SECTION + int1 - _begin), 0, 1
idt_int HEX(8), (BASE_OF_SECTION + int2 - _begin), 0, 1
idt_int HEX(8), (BASE_OF_SECTION + int3 - _begin), 0, 1
idt_int HEX(8), (BASE_OF_SECTION + int4 - _begin), 0, 1
idt_int HEX(8), (BASE_OF_SECTION + int5 - _begin), 0, 1
idt_int HEX(8), (BASE_OF_SECTION + int6 - _begin), 0, 1
idt_int HEX(8), (BASE_OF_SECTION + int7 - _begin), 0, 1
idt_int HEX(8), (BASE_OF_SECTION + int8 - _begin), 0, 1
idt_int HEX(8), (BASE_OF_SECTION + int9 - _begin), 0, 1
idt_int HEX(8), (BASE_OF_SECTION + int10 - _begin), 0, 1
idt_int HEX(8), (BASE_OF_SECTION + int11 - _begin), 0, 1
idt_int HEX(8), (BASE_OF_SECTION + int12 - _begin), 0, 1
idt_int HEX(8), (BASE_OF_SECTION + int13 - _begin), 0, 1
idt_int HEX(8), (BASE_OF_SECTION + int14 - _begin), 0, 1
idt_int HEX(8), (BASE_OF_SECTION + int15 - _begin), 0, 1
idt_int HEX(8), (BASE_OF_SECTION + int16 - _begin), 0, 1
idt_int HEX(8), (BASE_OF_SECTION + int17 - _begin), 0, 1
idt_int HEX(8), (BASE_OF_SECTION + int18 - _begin), 0, 1
idt_int HEX(8), (BASE_OF_SECTION + int19 - _begin), 0, 1

//REPEAT 236
idt_int 0x8, (BASE_OF_SECTION + empty_irq - _begin), 0, 1
//ENDR

idt_descriptor:
.word 256*8-1
.long _idt

exception_nr:
.long 0
