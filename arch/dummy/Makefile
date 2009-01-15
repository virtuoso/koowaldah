# dummy architecture provides a so-called loader which is
# a normal host executable
loader     := $(PRJROOT)/kos-loader
local_OBJS := \
	$(OBJDIR)/main.oH    \
	$(OBJDIR)/memory.oH  \
	$(OBJDIR)/threads.oH \
	$(OBJDIR)/timer.oH   \
	$(OBJDIR)/sysrq.oH   \
	$(OBJDIR)/dl_user.oH

# kernel images
kos_nosyms := $(OBJDIR)/kos-shared.0
kos_final  := $(PRJROOT)/kos-shared.so

pre-compile: $(PRJROOT)/confdefs

# architecture-specific kernel code
SOURCES := \
	   memory.c    \
	   threads.c   \
	   machine.c   \
	   sysrq.c     \
	   syscalls.c  \
	   serial.c


include $(PRJROOT)/tools/main.mk
LD_FLAGS += -shared

$(PRJROOT)/confdefs: tests/ucontext_size.c tests/page_shift.c
	: > $@
ifeq ($(MK_UCONTEXT_SIZE),0)
	$(HOSTCC) $(HOSTCC_FLAGS) $(HOSTLD_FLAGS) tests/ucontext_size.c \
		-o $(OBJDIR)/ucontext_size
	$(OBJDIR)/ucontext_size > $@
else
	echo -n " -DUCONTEXT_SIZE=$(MK_UCONTEXT_SIZE)" > $@
endif
ifeq ($(MK_PAGE_SHIFT),0)
	$(HOSTCC) $(HOSTCC_FLAGS) $(HOSTLD_FLAGS) tests/page_shift.c \
		-o $(OBJDIR)/page_shift
	$(OBJDIR)/page_shift >> $@
else
	echo -n " -DPAGE_SHIFT=$(MK_PAGE_SHIFT)" >> $@
endif

# how to build the loader
$(local_OBJS): $(patsubst $(OBJDIR)/%.oH,loader/%.c,$(local_OBJS))

$(OBJDIR)/%.oH:
	@$(call DO_HOSTCC,$(patsubst $(OBJDIR)/%.oH,loader/%.c,$@),$@,$(HOSTCC_FLAGS))

$(loader): $(local_OBJS)
	$(HOSTLD) $(HOSTLD_FLAGS) $(local_OBJS) -o $(loader) -Wl,-E -ldl

include $(PRJROOT)/tools/link-kernel.mk

link-kernel: $(loader)

all-local: $(loader)

CLEAN_FILES += $(loader) $(PRJROOT)/confdefs

