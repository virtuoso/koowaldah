# Koowaldah build system main makefile
# ------------------------------------

# Defines rules and functions that do most of the work.

-include konfig.mk
CC ?= gcc
LD ?= ld
ASM ?= gcc
MAKE ?= make

# some variables useful for building stuff
# ----------------------------------------
# current directory
THISDIR := $(patsubst /%,%,$(subst $(PRJROOT),,$(CURDIR)))
# list of local source files with full path prepended
FULLSOURCES := $(patsubst %,$(THISDIR)/%,$(SOURCES))
# interim list of resulting object files
OBJS := $(subst .S,.o,$(FULLSOURCES:.c=.o))
# list of resulting object files in $(OBJDIR)
OBJECTS := $(patsubst %,$(OBJDIR)/%,$(subst /,+,$(OBJS)))
# list of source dependency files in $(OBJDIR)
DEPS := $(OBJECTS:.o=.d)

# function for converting object file names into source file name
OBJ2SRC = $(subst .o,.c,$(subst $(OBJDIR)/$(subst /,+,$(THISDIR))+,,$(1)))
# the same, but convert dependency file names instead
DEP2SRC = $(subst +,/,$(subst .d,.c,$(subst $(OBJDIR)/$(subst /,+,$(THISDIR))+,,$(1))))

# functions that do actual work
# -----------------------------
# call compiler to generate dependencies file
# $(1) -- .c source
# $(2) -- .o output
# $(3) -- CFLAGS to use
DO_GENDEPS = echo -n "  GENDEPS  $(1)... "; \
	$(CC) -M $(3) -c $(1) -MT $(2:.d=.o) -MF $(2); \
	if [ $$? != "0" ]; then \
		echo "FAILED"; \
		false; \
	else \
		echo "OK"; \
	fi

# call compiler to generate an object file
# $(1) -- .c source
# $(2) -- .o output
# $(3) -- CFLAGS to use
DO_CC = echo -n "  CC  $(1)... "; \
	$(CC) $(3) -c $(1) -o $(2); \
	if [ $$? != "0" ]; then \
		echo "FAILED"; \
		false; \
	else \
		echo "OK"; \
	fi

# call linker
# $(1) -- list of objects to link
# $(2) -- output
# $(3) -- linker options
DO_LINK = echo -n "  LD $(2)... "; \
	$(LD) $(3) -o $(2) $(1); \
	if [ $$? != "0" ]; then \
		echo "FAILED"; \
		false; \
	else \
		echo "OK"; \
	fi

# call assembler
# $(1) -- input
# $(2) -- output
# $(3) -- options
DO_ASM =  echo -n "  AS $(1)... "; \
	$(ASM) -D__ASSEMBLY__ $(3) -c $(1) -o $(2); \
	if [ $$? != "0" ]; then \
		echo "FAILED"; \
		false; \
	else \
		echo "OK"; \
	fi

# rules to build, clean etc
# -------------------------
all: all-local

# make dependencies, objects and call itself for subdirectories
build: build-local deps objects

# make dependencies
deps: $(DEPS)
	@for d in $(SUBDIRS); do \
		$(MAKE) deps \
			PRJROOT=$(PRJROOT) \
			OBJDIR=$(OBJDIR) \
			CC=$(CC) \
			LD=$(LD) \
			ASM=$(ASM) \
			CC_FLAGS="$(CC_FLAGS)" \
			NODEPS=0 \
			-C $$d; \
	done


objects: $(OBJECTS) $(SUBDIRS)
	@for d in $(SUBDIRS); do \
		$(MAKE) build \
			PRJROOT=$(PRJROOT) \
			OBJDIR=$(OBJDIR) \
			CC=$(CC) \
			LD=$(LD) \
			ASM=$(ASM) \
			CC_FLAGS="$(CC_FLAGS)" \
			NODEPS=0 \
			-C $$d; \
	done

# clean -- removes current source dir's object and dependency
#          files from $(OBJDIR) and calls itself for all
#          subdirectories
clean:
	@rm -f $(OBJECTS) $(DEPS)
	@for d in $(SUBDIRS); do \
		$(MAKE) clean \
			PRJROOT=$(PRJROOT) \
			OBJDIR=$(OBJDIR) \
			CC=$(CC) \
			LD=$(LD) \
			ASM=$(ASM) \
			CC_FLAGS="$(CC_FLAGS)" \
			NODEPS=1 \
			-C $$d; \
	done
	@$(MAKE) clean-local \
			PRJROOT=$(PRJROOT) \
			OBJDIR=$(OBJDIR) \
			CC=$(CC) \
			LD=$(LD) \
			ASM=$(ASM) \
			CC_FLAGS="$(CC_FLAGS)" \
			NODEPS=1

$(OBJDIR)/%.d:
	@if [ -f "$(subst .c,.S,$(call DEP2SRC,$@))" ]; then \
		echo "$(@:.d=.o): $(subst .c,.S,$(call DEP2SRC,$@))" > $@; \
	else \
		$(call DO_GENDEPS,$(call DEP2SRC,$@),$@,-I$(PRJROOT)/include -ffreestanding); \
	fi

ifneq ($(NODEPS),1)
ifneq ($(DEPS),)
-include $(DEPS)
endif
endif

$(OBJDIR)/%.o:
	@if [ "$(subst .c,,$<)" != "$<" ]; then \
		$(call DO_CC,$<,$@,$(CC_FLAGS) -I$(PRJROOT)/include -ffreestanding); \
	else \
		$(call DO_ASM,$<,$@,$(ASM_FLAGS) -I$(PRJROOT)/include -ffreestanding); \
	fi

printobjs:
	@[ -n "$(OBJECTS)" ] && echo $(OBJECTS) >> $(OBJDIR)/OBJECTS || :
	@for d in $(SUBDIRS); do \
		$(MAKE) printobjs \
			PRJROOT=$(PRJROOT) \
			OBJDIR=$(OBJDIR) \
			CC=$(CC) \
			LD=$(LD) \
			ASM=$(ASM) \
			CC_FLAGS="$(CC_FLAGS)" \
			NODEPS=0 \
			-C $$d; \
	done

killobjs:
	rm -f $(OBJDIR)/OBJECTS

.PHONY: build build-local deps objects subdirs-deps subdirs-build clean clean-local printobjs init killobjs
