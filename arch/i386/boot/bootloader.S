;This file is based on Gregor Brunmar's 
;third tutorial, http://hem.passagen.se/gregge/

[BITS 16]
[ORG 0x7C00]


start:
reset_drive:			;Reset the boot drive
	
;        mov byte [0B8000h], '1'
;        mov byte [0B8001h], 0x0F

	mov ah, 0
	int 13h
	or ah, ah
	jnz reset_drive

;Now read the kernel from the
;second and third sectors
	
	mov ax, 0
	mov es, ax
	mov bx, 0x1000		;load kernel code at 1000h  
	
	mov ah, 02h		;"Read sector from disk" command
	mov al, 2Fh		;Read 32 sectors (16K)
	mov ch, 0		
	mov cl, 02h		;Begin with sector 2 (disk starts with sector 1)
	mov dh, 0
	int 13h			;Do it
	or ah, ah
	jnz reset_drive		

;Now we have our kernel loaded at adress 1000h. We must switch to 
;protected mode and set up segmentation
	
	in al, 92h
	or al, 2
	out 92h, al		;enable A20 gate
	
	cli
	xor ax, ax
	mov ds, ax
	lgdt [gdt_desc]		;Load the GTD-descriptor

	mov eax, cr0		;Enter protected mode
	or eax, 1
	mov cr0, eax

	jmp 08h:clear_pipe	;Clear the instruction pipe
[BITS 32]
clear_pipe:

	mov ax, 10h	
	mov ds, ax		;Set data segment

	mov ss, ax		;Set stack (in data segment)
	mov esp, 090000h

;	mov byte [0B8000h], '2'
;	mov byte [0B8001h], 0x0F

	jmp 08h:1000h		;jump into the kernel




gdt:                    ; Address for the GDT
gdt_null:               ; Null Segment
        dd 0
        dd 0

gdt_code:               ; Code segment, read/execute, nonconforming
        dw 0FFFFh
        dw 0
        db 0
        db 10011010b
        db 11001111b
        db 0

gdt_data:               ; Data segment, read/write, expand down
        dw 0FFFFh
        dw 0
        db 0
        db 10010010b
        db 11001111b
        db 0
	
gdt_end:                ; Used to calculate the size of the GDT


gdt_desc:                       ; The GDT descriptor
        dw gdt_end - gdt - 1    ; Limit (size)
        dd gdt                  ; Address of the GDT

times 510-($-$$) db 0
dw 0xAA55
