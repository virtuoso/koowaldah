
;
; arch/i386/asm/isr.S
;
; Copyright (C) 2005 Alexey Zaytsev
;
; Is distributed under the GPL 2 license.
;

global isr_table

%macro m1 1
global default_isr%1
default_isr%1:
	pushad
	push dword %1			;default_interrult_handler takes a lone
					;argument - interrupt number
	jmp quit_isr
%endmacro

[bits 32]

[section .text]

extern default_interrupt_handler

%assign i 0
%rep 256
	m1 i
%assign i i+1
%endrep

quit_isr:
	call default_interrupt_handler
	pop eax				;remove the int number
	popad			
	iret

[section .data]
isr_table:
times 1024 dw 0



