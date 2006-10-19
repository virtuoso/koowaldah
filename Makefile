
#### CONFIGURATION SECTION START ####


ARCH = i386


#### CONFIGURATION SECTION END ####

ASM ?= nasm
ASM_FLAGS += -g
CC ?= gcc
CC_FLAGS += -Wall -g 
LD ?= ld
OBJCOPY ?= objcopy


SRC = 	kernel/init.c \
	kernel/init/tests.c \
	kernel/textio.c \
	kernel/lib/klist.c \
	kernel/lib/klib.c \
	kernel/lib/vsnprintf.c \
	kernel/thread.c \
	kernel/scheduler.c \
	kernel/sched0.c \
	kernel/timecore.c \
	kernel/mm_zone.c \
	kernel/page_alloc.c \
	kernel/fs/super.c \
	kernel/fs/inode.c \
	kernel/fs/file.c \
	kernel/fs/namespace.c \
	kernel/fs/fs.c \
	drivers/keyboard/pc-kbd.c \
	arch/i386/bug.c \
	arch/i386/asm.c \
	arch/i386/pic.c \
	arch/i386/serial_console.c \
	arch/i386/vga_console.c \
	arch/i386/console.c \
	arch/i386/isr.c \
	arch/i386/irq.c \
	arch/i386/trap.c \
	arch/i386/timer.c \
	arch/i386/thread.c \
	arch/i386/bootmem_zone.c \
	arch/i386/mm.c \
	arch/i386/paging.c

OBJ = $(SRC:.c=.o)

OBJ +=	arch/i386/asm/isr.o \
	arch/i386/asm/irq.o \
	arch/i386/asm/context_switch.o

all: kernel tests

.c.o:
	$(CC) $(CC_FLAGS) -Iinclude -ffreestanding -c $< -o $@

.S.o:
	$(ASM) $(ASM_FLAGS) -f aout  $< -o $@

include/koptions.h: konfig/konfigure
	konfig/konfigure
	cat KONFIG

konfig/konfigure: konfig/konfigure.c
	$(CC) -Wall -Ikonfig -I. -o konfig/konfigure.o -c konfig/konfigure.c
	$(CC) -Wall -Ikonfig -I. -o konfig/kout.o -c konfig/kout.c
	$(CC) -Wall -Ikonfig -I. -o konfig/kuser.o -c konfig/kuser.c
	$(CC) -o $@ konfig/*.o

image:	kernel loader
	$(MAKE) -C rootfs

initfs:
	$(MAKE) -C usr
konfig: include/koptions.h
.PHONY: konfig

kernel-elf: $(OBJ) arch/i386/boot/multiboot.o
	$(LD) -T arch/i386/kernel-elf.lds -o kuca-elf \
		arch/i386/boot/multiboot.o $(OBJ) usr/rootfs.o
	
kernel: konfig initfs kernel-elf

tests:
	$(MAKE) -C tests test

retest:
	$(MAKE) clean test -C tests

loader: arch/i386/boot/bootloader.S
	$(ASM) -f bin arch/i386/boot/bootloader.S -o arch/i386/boot/bootloader.bin

clean:
	$(MAKE) clean -C rootfs
	$(MAKE) clean -C usr
	$(MAKE) clean -C tests
	rm -f kernel/kernel.bin kuca-bin kuca-elf
	rm -f arch/i386/boot/bootloader.bin
	rm -fr arch/i386/*.o arch/i386/asm/*.o arch/i386/boot/*.o
	rm -fr *.o kernel/*.o kernel/libs/*.o drivers/*.o kernel/fs/*.o
	rm -fr kernel/init/*.o kernel/lib/*.o
	rm -fr drivers/*.o drivers/keyboard/*.o
	rm -f include/koptions.h konfig/konfigure konfig/*.o konfig.mk

