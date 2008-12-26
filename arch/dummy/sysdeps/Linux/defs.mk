# set Linux-specific compiler's and linker's command line options here,
# using the following variables:
#  * SYSDEPSHOSTCC       -- default host c compiler
#  * SYSDEPSHOSTCC_FLAGS -- flags passed to host compiler $(HOSTCC)
#  * SYSDEPSHOSTLD       -- default host linker
#  * SYSDEPSHOSTLD_FLAGS -- flags passed to host linker $(HOSTLD)
#  * SYSDEPSCC_FLAGS     -- flags passed to compiler $(CC)
#  * SYSDEPSLD_FLAGS     -- flags passed to linker $(LD)

SYSDEPSHOSTCC :=
SYSDEPSHOSTCC_FLAGS += -DBUILD_ON_LINUX=1
SYSDEPSCC_FLAGS += -DTHIS_IS_X86_KERNEL=1
SYSDEPSLD_FLAGS +=
SYSDEPSHOSTLD :=
SYSDEPSHOSTLD_FLAGS +=

