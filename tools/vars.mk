#
# Various variables are set here.
#

include $(TOOLSDIR)/version.mk

VERSION_STRING = $(VERMAJOR).$(VERMINOR).$(VEREXTRA)$(MK_VERSION_APPEND)

ifeq ($(MK_CPU_ARCH_DUMMY),Y)
ARCH := dummy
endif
ifeq ($(MK_CPU_ARCH_I386),Y)
ARCH := i386
endif
ifeq ($(MK_CPU_ARCH_POWERPC),Y)
ARCH := powerpc
endif
ifeq ($(MK_CPU_ARCH_ARM),Y)
ARCH := arm
endif

HOSTCC	= gcc
HOSTLD	= gcc
CC	= $(CROSS_COMPILE)gcc
LD	= $(CROSS_COMPILE)ld
ASM	= $(CROSS_COMPILE)gcc
OBJCOPY = $(CROSS_COMPILE)objcopy
AR	= $(CROSS_COMPILE)ar

SYS_HOSTCC_FLAGS = -Wall -g
SYS_HOSTLD_FLAGS =
SYS_CC_FLAGS = -Werror -Wall -ffreestanding -nostdinc -nostdlib \
		-g -O2 -std=gnu89 -fno-strict-aliasing -funsigned-bitfields \
		-funsigned-char -fno-asm -fno-builtin -fno-cond-mismatch \
		-fno-force-addr -finline-limit=1200 -fno-omit-frame-pointer \
		-DVERSION_STRING=\"$(VERSION_STRING)\" \
		$(shell [ ! -f $(PRJROOT)/confdefs ] || cat $(PRJROOT)/confdefs)
SYS_LD_FLAGS =
SYS_ASM_FLAGS = -Wall -ffreestanding -nostdinc -nostdlib -g

HOST_MACH := $(shell $(HOSTCC) -dumpmachine | cut -d - -f 1)
HOST_OS   := $(shell uname)

# add per-machine and per-os defines should such exist
-include $(PRJROOT)/arch/$(ARCH)/sysdeps/$(HOST_MACH)-defs.mk
-include $(PRJROOT)/arch/$(ARCH)/sysdeps/$(HOST_OS)-defs.mk

HOSTCC_FLAGS = $(SYS_HOSTCC_FLAGS)
HOSTLD_FLAGS = $(SYS_HOSTLD_FLAGS)
CC_FLAGS = $(SYS_CC_FLAGS)
LD_FLAGS = $(SYS_LD_FLAGS)
ASM_FLAGS = $(SYS_ASM_FLAGS)

