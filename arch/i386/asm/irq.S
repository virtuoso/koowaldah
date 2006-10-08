
;
; arch/i386/asm/irq.S
;
; Copyright (C) 2005 Alexey Zaytsev
;
; Is distributed under the GPL 2 license.
;

%macro m1 1
global irq_pre_handler%1
irq_pre_handler%1:
	pushad
	push dword %1			;main_irq_handler takes a lone
					;argument - interrupt number
	jmp quit_irq
%endmacro

[bits 32]

[section .text]

extern main_irq_handler 		;irq.c

%assign i 0
%rep 16
	m1 i
%assign i i+1
%endrep

quit_irq:
	call main_irq_handler
	pop eax				;remove the int number
	popad			
	iret

