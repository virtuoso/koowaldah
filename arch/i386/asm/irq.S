
;
; arch/i386/asm/irq.S
;
; Copyright (C) 2005 Alexey Zaytsev
;
;  Redistribution and use in source and binary forms, with or without
;  modification, are permitted provided that the following conditions
;  are met:
; 
;  1. Redistributions of source code must retain the above copyright
;     notice, this list of conditions and the following disclaimer.
;  2. Redistributions in binary form must reproduce the above copyright
;     notice, this list of conditions and the following disclaimer in the
;     documentation and/or other materials provided with the distribution.
;  3. Neither the name of the Koowaldah developers nor the names of theyr 
;     contributors may be used to endorse or promote products derived from
;     this software without specific prior written permission.
; 
;  THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
;  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
;  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
;  ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
;  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
;  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
;  OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
;  HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
;  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
;  OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
;  SUCH DAMAGE.
;  
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

