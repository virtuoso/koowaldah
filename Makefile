PRJROOT := $(shell echo $(CURDIR))

include tools/main.mk

#### CONFIGURATION SECTION START ####

ifeq ($(MK_CPU_ARCH_I386),Y)
ARCH := i386
endif
ifeq ($(MK_CPU_ARCH_POWERPC),Y)
ARCH := powerpc
endif
ifeq ($(MK_CPU_ARCH_ARM),Y)
ARCH := arm
endif

#### CONFIGURATION SECTION END ####

ifeq ($(MK_GLOBAL_DEBUG),Y)
  CC_FLAGS += -DDEBUG=1
endif
ifeq ($(MK_WERROR),Y)
  CC_FLAGS += -Werror
endif

ifeq ($(MK_INSTRUMENT_PROFILER),Y)
  CC_FLAGS += -finstrument-functions
endif

OBJDIR := $(PRJROOT)/solver

ifeq ($(MK_INSTRUMENT_PROFILER),Y)
  SRC += kernel/debug/profile.c
endif

all-local: kernel

include/koptions.h: konfig/konfigure
	konfig/konfigure
	mkdir -p $(OBJDIR)
	cat KONFIG

konfig/konfigure: konfig/konfigure.c
	$(HOSTCC) -Wall -Ikonfig -I. -o konfig/konfigure.o -c konfig/konfigure.c
	$(HOSTCC) -Wall -Ikonfig -I. -o konfig/kout.o -c konfig/kout.c
	$(HOSTCC) -Wall -Ikonfig -I. -o konfig/ktree.o -c konfig/ktree.c
	$(HOSTCC) -Wall -Ikonfig -I. -o konfig/kuser.o -c konfig/kuser.c
	$(HOSTCC) -o $@ konfig/*.o

image:	kernel
	$(MAKE) -C rootfs

initfs:
	$(MAKE) -C usr
	echo usr/rootfs.o >> $(OBJDIR)/OBJECTS

konfig: include/koptions.h

SUBDIRS := arch/$(ARCH) kernel drivers/keyboard drivers/serial
kernel-elf: deps objects printobjs
	cat $(OBJDIR)/OBJECTS | \
		xargs $(LD) -T arch/$(ARCH)/kernel-elf.lds -o kos-elf
	
kernel: killobjs konfig initfs
	$(MAKE) -R kernel-elf

tests:
	$(MAKE) -C tests test

retest:
	$(MAKE) clean test -C tests

clean-local:
	$(MAKE) clean -C rootfs
	$(MAKE) clean -C usr
	$(MAKE) clean -C tests
	rm -rf $(OBJDIR)
	rm -f kos-elf
	rm -f include/koptions.h konfig/konfigure konfig/*.o konfig.mk

.PHONY: konfig tests image initfs kernel-elf
.NOTPARALLEL: konfig initfs tests
