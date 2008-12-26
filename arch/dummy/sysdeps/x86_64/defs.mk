# set x86_64-specific compiler's and linker's command line options here,
# using the following variables:
#  * SYSDEPSHOSTCC       -- default host c compiler
#  * SYSDEPSHOSTCC_FLAGS -- flags passed to host compiler $(HOSTCC)
#  * SYSDEPSHOSTLD       -- default host linker
#  * SYSDEPSHOSTLD_FLAGS -- flags passed to host linker $(HOSTLD)
#  * SYSDEPSCC_FLAGS     -- flags passed to compiler $(CC)
#  * SYSDEPSLD_FLAGS     -- flags passed to linker $(LD)

SYSDEPSHOSTCC :=
SYSDEPSHOSTCC_FLAGS += -m32
SYSDEPSLD_FLAGS += -m elf_i386
SYSDEPSCC_FLAGS += -m32
SYSDEPSHOSTLD :=
SYSDEPSHOSTLD_FLAGS +=

