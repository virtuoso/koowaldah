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

# image loader sources
# 16bit startup code
LO_SOURCES16 :=              \
	loader/kernel.S      \
	loader/linux_proto.S \
	loader/loader.c      \
	loader/setup.S       \
	loader/vga.c

# 32bit code
LO_SOURCES :=                \
	loader/moowaldah.c

LO_CFLAGS :=                 \
	-nostdinc            \
	-march=i386          \
	-fno-unit-at-a-time  \
	-fno-strict-aliasing \
	-fomit-frame-pointer \
	-fno-stack-protector \
	-g3 -Os              \
	-Iloader             \
	-I$(PRJROOT)/include \
	-ffreestanding       \
	-DLO_CORE            \
	-DKERNSZ=`stat -c %s $(OBJDIR)/kos-bin`

LO_CFLAGS16 := $(LO_CFLAGS)  \
	-mpreferred-stack-boundary=2 \
	-DBOOT_GDT_ENTRIES=3 \
	-I$(PRJROOT)/kernel/lib \
	-DKOS_BIN_PATH=\"$(OBJDIR)/kos-bin\"
#-DKOS_ENTRY=`$(OBJDUMP) -t $(kos_final) | grep multiboot_start | ` \

LO_CFLAGS32 := $(LO_CFLAGS)  \
	-I. \
	-DSYMTAB_PATH=\"$(OBJDIR)/symtab.c\" \
	-DBITS32

loader     := $(PRJROOT)/kos-image
local_OBJS := \
	$(patsubst loader/%,$(OBJDIR)/%.16.oL,$(LO_SOURCES16)) \
	$(patsubst loader/%,$(OBJDIR)/%.32.oL,$(LO_SOURCES))

# how to build the loader
$(local_OBJS): $(OBJDIR)/kos-bin $(LO_SOURCES16) $(LO_SOURCES)

$(OBJDIR)/kos-bin: $(kos_final)
	$(OBJCOPY) -S -g -O binary $(kos_final) $@

$(OBJDIR)/%.S.16.oL:
	@$(call DO_ASM, \
		$(patsubst $(OBJDIR)/%.16.oL,loader/%,$@), \
		$@,$(LO_CFLAGS16))

$(OBJDIR)/%.c.16.oL:
	@$(call DO_CC, \
		$(patsubst $(OBJDIR)/%.16.oL,loader/%,$@), \
		$@,$(LO_CFLAGS16))

$(OBJDIR)/%.c.32.oL:
	@$(call DO_CC, \
		$(patsubst $(OBJDIR)/%.32.oL,loader/%,$@), \
		$@,$(LO_CFLAGS32))

$(loader): $(local_OBJS)
	$(LD) $(local_OBJS) -static -z defs -nostdlib -Tloader/loader.lds -o $(OBJDIR)/kos-image-elf
	$(OBJCOPY) -S -g -O binary $(OBJDIR)/kos-image-elf $@

link-kernel: $(loader)

LD_FLAGS := -T kernel-elf.lds -z defs
include $(PRJROOT)/tools/link-kernel.mk

