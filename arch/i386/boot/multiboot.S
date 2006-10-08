
;
; arch/i386/bot/multiboot.S
;
; Copyright (C) 2005 Alexey Zaytsev
;
; Is distributed under the GPL 2 license.
;

extern kern_start
extern kernel_stack
global multiboot_start

; setting up the Multiboot header - see GRUB docs for details
MODULEALIGN equ  1<<0                   ; align loaded modules on page boundaries
MEMINFO     equ  1<<1                   ; provide memory map
FLAGS       equ  MODULEALIGN | MEMINFO  ; this is the Multiboot 'flag' field
MAGIC       equ    0x1BADB002           ; 'magic number' lets bootloader find the header
CHECKSUM    equ -(MAGIC + FLAGS)        ; checksum required

section .text
align 4
MultiBootHeader:
   dd MAGIC
   dd FLAGS
   dd CHECKSUM

multiboot_start:
	mov esp, kernel_stack
	jmp kern_start
	cli
	hlt

