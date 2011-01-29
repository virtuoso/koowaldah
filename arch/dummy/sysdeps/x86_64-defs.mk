
# x86_64-specific make variable overrides.

# You can alter HOSTCC_FLAGS HOSTLD_FLAGS CC_FLAGS, LD_FLAGS
# and ASM_FLAGS usign the SYS_ prefixed versions.

SYS_HOSTCC_FLAGS += -m32
SYS_HOSTLD_FLAGS += -m32
SYS_LD_FLAGS += -m elf_i386
SYS_CC_FLAGS += -m32
SYS_ASM_FLAGS += -m32

