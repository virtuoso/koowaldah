#
# Various variables are set here.
#

CROSS_COMPILE  ?=

HOSTCC          = gcc

CC              = $(CROSS_COMPILE)gcc
CC_FLAGS       += -Wall -ffreestanding -nostdinc -nostdlib -g -std = gnu89
CC_FLAGS_KERN  := $(CC_FLAGS)

LD              = $(CROSS_COMPILE)ld

ASM             = $(CROSS_COMPILE)gcc
ASM_FLAGS      += -Wall -ffreestanding -nostdinc -nostdlib -g

OBJCOPY         = $(CROSS_COMPILE)objcopy
AR              = $(CROSS_COMPILE)ar
# MAKE ?        = make

export CROSS_COMPILE HOSTCC CC CC_FLAGS CC_FLAGS_KERN LD \
       ASM ASM_FLAGS OBJCOPY AR
