
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




