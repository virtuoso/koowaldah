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
	mm.c \
	paging.c \
	segments.c \
	asm/syscall.S \
	asm/irq.S \
	asm/context_switch.S

build-local:
	for i in `seq 0 255`; do \
		echo "default_isr($$i)"; \
	done > asm/isr_table.h

clean-local:
	rm -f asm/isr_table.h

include $(PRJROOT)/tools/main.mk

