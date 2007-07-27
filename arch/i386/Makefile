# kernel images
kos_nosyms := $(OBJDIR)/kos-elf.0
kos_final  := $(PRJROOT)/kos-elf

# architecture-specific kernel code
SOURCES := \
	boot/multiboot.S \
	syscalls.c \
	bug.c \
	asm.c \
	pic.c \
	serial_console.c \
	vga_console.c \
	console.c \
	irq.c \
	trap.c \
	timer.c \
	thread.c \
	bootmem_zone.c \
	paging.c \
	segments.c \
	asm/interrupt.S \
	asm/irq.S \
	asm/context_switch.S

include $(PRJROOT)/tools/main.mk

LD_FLAGS := -T kernel-elf.lds -z defs
include $(PRJROOT)/tools/link-kernel.mk

