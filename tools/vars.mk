#
# Various variables are set here.
#

CROSS_COMPILE  ?=

HOSTCC          = gcc

CC              = $(CROSS_COMPILE)gcc
ifndef CC_FLAGS
CC_FLAGS        = -Werror -Wall -ffreestanding -nostdinc -nostdlib -g -O2 -std=gnu89 -fno-strict-aliasing -funsigned-bitfields -funsigned-char -fno-asm -fno-builtin -fno-cond-mismatch -fno-force-addr -finline-limit=1200 -fno-omit-frame-pointer $(shell [ ! -f $(PRJROOT)/confdefs ] || cat $(PRJROOT)/confdefs)
endif

CC_FLAGS_KERN  := $(CC_FLAGS)

LD              = $(CROSS_COMPILE)ld

ASM             = $(CROSS_COMPILE)gcc
ASM_FLAGS      += -Wall -ffreestanding -nostdinc -nostdlib -g

OBJCOPY         = $(CROSS_COMPILE)objcopy
AR              = $(CROSS_COMPILE)ar
# MAKE ?        = make

export CROSS_COMPILE HOSTCC CC CC_FLAGS CC_FLAGS_KERN LD LD_FLAGS \
       ASM ASM_FLAGS OBJCOPY AR
