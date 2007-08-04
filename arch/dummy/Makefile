# dummy architecture provides a so-called loader which is
# a normal host executable
loader     := $(PRJROOT)/kos-loader
local_OBJS := $(OBJDIR)/main.oH $(OBJDIR)/memory.oH $(OBJDIR)/threads.oH

# kernel images
kos_nosyms := $(OBJDIR)/kos-shared.0
kos_final  := $(PRJROOT)/kos-shared.so

pre-compile: $(PRJROOT)/confdefs

$(PRJROOT)/confdefs: tests/ucontext_size.c tests/page_shift.c
	$(CC) tests/ucontext_size.c -o $(OBJDIR)/ucontext_size
	$(CC) tests/page_shift.c -o $(OBJDIR)/page_shift
	$(OBJDIR)/ucontext_size > $@
	$(OBJDIR)/page_shift >> $@

# architecture-specific kernel code
SOURCES := \
	   late_init.c \
	   memory.c    \
	   threads.c   \
	   machine.c

include $(PRJROOT)/tools/main.mk

# how to build the loader
$(local_OBJS): $(patsubst $(OBJDIR)/%.oH,loader/%.c,$(local_OBJS))

$(OBJDIR)/%.oH:
	@$(call DO_CC,$(patsubst $(OBJDIR)/%.oH,loader/%.c,$@),$@,-Wall)

$(loader): $(local_OBJS)
	$(CC) $(local_OBJS) -o $(loader) -Wl,-E -ldl

LD_FLAGS := -shared
include $(PRJROOT)/tools/link-kernel.mk

link-kernel: $(loader)

all-local: $(loader)

CLEAN_FILES += $(loader) $(PRJROOT)/confdefs

