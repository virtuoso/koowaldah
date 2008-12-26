#
# Various variables are set here.
#

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

CROSS_COMPILE  ?=

ifndef HOSTCC
HOSTCC		:= gcc
endif

HOST_MACH := $(shell $(HOSTCC) -dumpmachine | sed -e 's,\(^[^-]\+\)-.*,\1,')
HOST_OS   := $(shell uname)

# add per-machine and per-os defines should such exist
-include $(PRJROOT)/arch/$(ARCH)/sysdeps/$(HOST_MACH)/defs.mk
-include $(PRJROOT)/arch/$(ARCH)/sysdeps/$(HOST_OS)/defs.mk

# check or sysdeps-defined CC
ifndef HOSTCC
ifdef SYSDEPSHOSTCC
HOSTCC		= $(SYSDEPSHOSTCC)
else
HOSTCC		= gcc
endif
endif

# check for sysdeps-defined LD
ifndef HOSTLD
ifdef SYSDEPSHOSTLD
HOSTLD		= $(SYSDEPSHOSTLD)
else
HOSTLD		= gcc
endif
endif

# include sysdeps-defined flags
HOSTCC_FLAGS	= -Wall -g $(SYSDEPSHOSTCC_FLAGS)
HOSTLD_FLAGS	= $(SYSDEPSHOSTLD_FLAGS)

CC              = $(CROSS_COMPILE)gcc
ifndef CC_FLAGS
CC_FLAGS        = -Werror -Wall -ffreestanding -nostdinc -nostdlib -g -O2 -std=gnu89 -fno-strict-aliasing -funsigned-bitfields -funsigned-char -fno-asm -fno-builtin -fno-cond-mismatch -fno-force-addr -finline-limit=1200 -fno-omit-frame-pointer $(shell [ ! -f $(PRJROOT)/confdefs ] || cat $(PRJROOT)/confdefs) $(SYSDEPSCC_FLAGS)

# Add versioning tags
-include $(TOOLSDIR)/version.mk

ifdef KOOWALDAH_VERMAJOR
VERSION_STRING:=$(KOOWALDAH_VERMAJOR).$(KOOWALDAH_VERMINOR).$(KOOWALDAH_VEREXTRA)
ifneq (,$(MK_VERSION_APPEND))
VERSION_STRING:= $(subst $(VERSION_STRING),$(VERSION_STRING),$(VERSION_STRING)-$(MK_VERSION_APPEND))
endif

CC_FLAGS += \
	-DVERMAJOR=$(KOOWALDAH_VERMAJOR) \
	-DVERMINOR=$(KOOWALDAH_VERMINOR) \
	-DVEREXTRA=$(KOOWALDAH_VEREXTRA) \
	-DVERSION_STRING=\"$(VERSION_STRING)\"
endif
endif

CC_FLAGS_KERN  := $(CC_FLAGS)

LD              = $(CROSS_COMPILE)ld

ifndef LD_FLAGS
LD_FLAGS	:= $(SYSDEPSLD_FLAGS)
endif

ASM             = $(CROSS_COMPILE)gcc
ASM_FLAGS      += -Wall -ffreestanding -nostdinc -nostdlib -g

OBJCOPY         = $(CROSS_COMPILE)objcopy
AR              = $(CROSS_COMPILE)ar
# MAKE ?        = make

export CROSS_COMPILE HOSTCC CC CC_FLAGS CC_FLAGS_KERN LD LD_FLAGS \
       ASM ASM_FLAGS OBJCOPY AR HOSTLD HOSTCC_FLAGS HOSTLD_FLAGS
