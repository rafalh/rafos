#include "macros.inc"

[SECTION .text]

[GLOBAL _irq0]
_irq0:
  push gs
  push fs
  push es
  push ds
  pusha
  ;mov ax, 0x10
  ;mov ds, ax
  ;mov es, ax
  mov eax, [IRQ0_fractions]
  mov ebx, [IRQ0_ms]
  add dword [sys_timer_fr], eax
  adc dword [sys_timer_ms], ebx
[EXTERN _timer_irq]
  call _timer_irq

  mov al, 0x20 ; EOI
  out 0x20, al

  popa
  pop ds
  pop es
  pop fs
  pop gs

  iretd

[GLOBAL _set_pit_freq]
_set_pit_freq:               ; (unsigned hz)
  push eax
  push ebx
  push edx

  mov ebx, [esp+16]

  ;ccall printf, str, ebx
; Do some checking
  mov eax, 0x10000                   ;eax = reload value for slowest possible frequency (65536)
  cmp ebx, 18                        ;Is the requested frequency too low?
  jbe .gotReloadValue                ; yes, use slowest possible frequency

  mov eax, 1                         ;ax = reload value for fastest possible frequency (1)
  cmp ebx, 1193181                   ;Is the requested frequency too high?
  jae .gotReloadValue                ; yes, use fastest possible frequency

  ;ccall printf, str_test, 0

; Calculate the reload value
  mov eax, 3579545
  mov edx, 0                         ;edx:eax = 3579545
  div ebx                            ;eax = 3579545 / frequency, edx = remainder
  cmp edx, 3579545 / 2               ;Is the remainder more than half?
  jb .l1                             ; no, round down
  inc eax                            ; yes, round up
.l1:
  mov ebx, 3
  mov edx, 0                         ;edx:eax = 3579545 * 256 / frequency
  div ebx                            ;eax = (3579545 * 256 / 3 * 256) / frequency
  cmp edx, 3 / 2                     ;Is the remainder more than half?
  jb .l2                             ; no, round down
  inc eax                            ; yes, round up
.l2:


; Store the reload value and calculate the actual frequency
.gotReloadValue:
  push eax                           ;Store reload_value for later
  mov [PIT_reload_value], eax         ;Store the reload value for later

  mov ebx, eax                       ;ebx = reload value

  mov eax, 3579545
  mov edx, 0                         ;edx:eax = 3579545
  div ebx                            ;eax = 3579545 / reload_value, edx = remainder
  cmp edx, 3579545 / 2               ;Is the remainder more than half?
  jb .l3                             ; no, round down
  inc eax                            ; yes, round up
.l3:
  mov ebx, 3
  mov edx, 0                         ;edx:eax = 3579545 / reload_value
  div ebx                            ;eax = (3579545 / 3) / frequency
  cmp edx, 3 / 2                     ;Is the remainder more than half?
  jb .l4                             ; no, round down
  inc eax                            ; yes, round up
.l4:
  mov [IRQ0_frequency], eax          ;Store the actual frequency for displaying later

; Calculate the amount of time between IRQs in 32.32 fixed point
;
; Note: The basic formula is:
;           time in ms = reload_value / (3579545 / 3) * 1000

  pop ebx                            ;ebx = reload_value
  mov eax, 0xDBB3A062                ;eax = 3000 * (2^42) / 3579545
  mul ebx                            ;edx:eax = reload_value * 3000 * (2^42) / 3579545
  shrd eax, edx, 10
  shr edx, 10                        ;edx:eax = reload_value * 3000 * (2^42) / 3579545 / (2^10)

  mov [IRQ0_ms], edx                 ;Set whole mS between IRQs
  mov [IRQ0_fractions], eax          ;Set fractions of 1 mS between IRQs

; Program the PIT channel
  pushfd
  cli                               ;Disabled interrupts (just in case)

  mov al, 00110100b                  ;channel 0, lobyte/hibyte, rate generator
  out 0x43, al

  mov ax, [PIT_reload_value]         ;ax = 16 bit reload value
  out 0x40, al                       ;Set low byte of PIT reload value
  mov al, ah                         ;ax = high 8 bits of reload value
  out 0x40, al                       ;Set high byte of PIT reload value

  popfd
  pop edx
  pop ebx
  pop eax

  ret

[GLOBAL _GetRTCTime]
_GetRTCTime:
.l1:
  mov al, 10        ;Get RTC register A
	out 0x70, al
	in al, 0x71
	test al, 0x80			;Is update in progress?
	jne .l1				    ; yes, wait

	mov al, 0         ;Get seconds (00 to 59)
	out 0x70, al
	in al, 0x71
	mov [_rtc_second], al

	mov al, 0x02			;Get minutes (00 to 59)
	out 0x70, al
	in al, 0x71
	mov [_rtc_minute], al

	mov al, 0x04			;Get hours (see notes)
	out 0x70, al
	in al, 0x71
	mov [_rtc_hour], al

	mov al, 0x07			;Get day of month (01 to 31)
	out 0x70, al
	in al, 0x71
	mov [_rtc_day], al

	mov al, 0x08			;Get month (01 to 12)
	out 0x70, al
	in al, 0x71
	mov [_rtc_month], al

	mov al, 0x09			;Get year (00 to 99)
	out 0x70, al
	in al, 0x71
	mov [_rtc_year], al
	ret

global _KeTickCount
export KeTickCount
_KeTickCount:
  mov eax, [sys_timer_ms]
  ret

[SECTION .data]

sys_timer_ms dd 0
sys_timer_fr dd 0

[GLOBAL _rtc_second]
_rtc_second db 0
[GLOBAL _rtc_minute]
_rtc_minute db 0
[GLOBAL _rtc_hour]
_rtc_hour db 0
[GLOBAL _rtc_day]
_rtc_day db 0
[GLOBAL _rtc_month]
_rtc_month db 0
[GLOBAL _rtc_year]
_rtc_year db 0

IRQ0_fractions dd 0         ; Fractions of 1 mS between IRQs
IRQ0_ms dd 50                ; Number of whole mS between IRQs
IRQ0_frequency dd 50         ; Actual frequency of PIT
PIT_reload_value dd 0

