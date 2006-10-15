
;
;
;  kernel/init.c
; 
;  Copyright (C) 2006 Alexey Zaytsev
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

global do_thread_switch_context
global do_thread_switch_to

do_thread_switch_context:
	pushf 
	pusha
;	mov ebx, [esp + 32 + 4]
;	mov ecx, [esp + 32 + 8]

	mov eax, [esp + 36 + 4]
	mov [eax], esp
	mov eax, [esp + 36 + 8]
	mov esp, [eax]
	popa
	popf 
	ret

;This functions does not save the current conteext. Its only purpose is to 
;run main kernel thread during system initialization.

do_thread_switch_to:
	pushf
	pusha
	mov eax, [esp + 36 + 4]
	mov esp, eax
	popa
	popf
	ret
	

;context_switch:
;	mov ecx, [esp + 4]
;	mov edx, [esp + 8]

;	mov [edx], esp
;	mov esp, ecx
	
;	ret




