
# Linux-specific make variable overrides.

# You can alter HOSTCC_FLAGS HOSTLD_FLAGS CC_FLAGS, LD_FLAGS
# and ASM_FLAGS usign the SYS_ prefixed versions.

SYS_HOSTCC_FLAGS += -D__LINUX__=1

