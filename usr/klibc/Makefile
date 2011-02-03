#MODULE := klibc

KOOWALDAH_DIR ?= $(PRJROOT)
ifeq ($(TOOLSDIR),)
  TOOLSDIR := $(KOOWALDAH_DIR)
endif

OBJDIR ?= $(PRJROOT)/solver

KLIBCDIR ?= $(shell pwd)/../../
DESTDIR ?= /tmp
KLIBCARCH ?= i386
KLIBCBITSIZE ?= 32

KLIBCDEFS += -D__KLIBC__=$(KLIBCMAJOR)	\
	-D__KLIBC_MINOR__=$(KLIBCMINOR)	\
	-D_BITSIZE=$(KLIBCBITSIZE)

KLIBCINCLUDES :=  -I$(PRJROOT)/usr/include/bits$(KLIBCBITSIZE) -I$(PRJROOT)/usr/include/arch/$(KLIBCARCH) -I$(KOOWALDAH_DIR)/include -I$(PRJROOT)/usr/include -I$(PRJROOT)/include/khui
SYS_CC_FLAGS := -Wall -g -ffreestanding -fno-stack-protector -nostdinc -nostdlib $(KLIBCINCLUDES) $(KLIBCDEFS) -O2 -std=gnu89 -funsigned-bitfields -funsigned-char

SOURCES :=                           \
	arch/$(KLIBCARCH)/setjmp.S   \
	arch/$(KLIBCARCH)/crt0.S     \
	arch/$(KLIBCARCH)/syscall.S  \
	getenv.c                     \
	globals.c                    \
	atoi.c	                     \
	atol.c	                     \
	atoll.c	                     \
	bsearch.c                    \
	brk.c                        \
	sbrk.c                       \
	malloc.c                     \
	realloc.c                    \
	sscanf.c                     \
	vsscanf.c                    \
	strtok.c                     \
	strntoumax.c                 \
	strtoull.c                   \
	strcspn.c                    \
	strcat.c                     \
	strncat.c                    \
	strchr.c                     \
	strcmp.c                     \
	strncmp.c                    \
	strncpy.c                    \
	strcpy.c                     \
	strtol.c                     \
	strtoul.c                    \
	strtoll.c                    \
	strstr.c                     \
	strspn.c                     \
	strxspn.c                    \
	strerror.c                   \
	perror.c                     \
	putchar.c                    \
	puts.c                       \
	memccpy.c                    \
	memchr.c                     \
	memcmp.c                     \
	memcpy.c                     \
	memmem.c                     \
	memmove.c                    \
	memrchr.c                    \
	memswap.c                    \
	ctype/isalnum.c              \
	ctype/isalpha.c              \
	ctype/isascii.c              \
	ctype/isblank.c              \
	ctype/iscntrl.c              \
	ctype/isdigit.c              \
	ctype/isgraph.c              \
	ctype/islower.c              \
	ctype/isprint.c              \
	ctype/ispunct.c              \
	ctype/isspace.c              \
	ctype/isupper.c              \
	ctype/isxdigit.c             \
	ctype/tolower.c              \
	ctype/toupper.c              \
	ctypes.c                     \
	qsort.c                      \
	open.c                       \
	mpipe_open.c                 \
	read.c                       \
	readdir.c                    \
	write.c                      \
	close.c                      \
	fork.c                       \
	exit.c                       \
	getpid.c                     \
	tsleep.c                     \
	yield.c                      \
	arch/$(KLIBCARCH)/syscalls.c \
	debug.c                      \
	sprintf.c                    \
	strlen.c                     \
	strrchr.c                    \
	strpbrk.c                    \
	snprintf.c                   \
	vsnprintf.c                  \
	memset.c                     \
	fopen.c                      \
	fread.c                      \
	fread2.c                     \
	fputc.c                      \
	fputs.c                      \
	fgetc.c                      \
	fgets.c                      \
	printf.c                     \
	fprintf.c                    \
	vfprintf.c                   \
	fwrite.c                     \
	fwrite2.c                    \
	__static_init.c              \
	libgcc/__umoddi3.c           \
	libgcc/__udivdi3.c           \
	libgcc/__udivmoddi4.c

all-local: deps printobjs objects $(OBJDIR)/libc.a
#all-local: $(OBJDIR)/libc.a

$(OBJDIR)/libc.a: $(OBJDIR)/OBJECTS-$(MODULE)
	$(AR) cru $@ $(shell cat $(OBJDIR)/OBJECTS-$(MODULE))

install: $(OBJDIR)/libc.a
	@echo "Installing klibc to $(DESTDIR)"
	@[ -d "$(DESTDIR)/include" ] || mkdir -p $(DESTDIR)/include
	@[ -d "$(DESTDIR)/lib" ] || mkdir -p $(DESTDIR)/lib
	@[ -d "$(DESTDIR)/ldscripts" ] || mkdir -p $(DESTDIR)/ldscripts
	@( cd $(KLIBCDIR)/usr; tar cf - include ) | ( cd $(DESTDIR); tar xf - )
	@cp -a $(KOOWALDAH_DIR)/include/khui $(DESTDIR)/include/
	@cp koowaldah-aout.lds $(DESTDIR)/ldscripts
	@cp koowaldah-elf.lds $(DESTDIR)/ldscripts
	@cp $(OBJDIR)/libc.a $(DESTDIR)/lib
	@cp $(OBJDIR)/usr+klibc+arch+$(KLIBCARCH)+crt0.o $(DESTDIR)/lib/crt0.o

clean-local:
	-rm -f libc.a

#-include $(PRJROOT)/konfig.mk

include $(TOOLSDIR)/tools/main.mk

