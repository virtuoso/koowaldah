include $(PRJROOT)/tools/main.mk

ifeq ($(MK_DUMMY_USE_DLOPEN),Y)
  CFLAGS   := -m32 -fno-stack-protector
  LDSCRIPT := -T$(OBJDIR)/klibc/ldscripts/koowaldah-elf.lds
  LDOPTS   := -shared -melf_i386 -z defs -z now --sort-common
else
  CFLAGS   :=
  LDSCRIPT := -T$(OBJDIR)/klibc/ldscripts/koowaldah-aout.lds
  LDOPTS   := --omagic
endif

# XXX
MKKLIBCCONFIG = sed \
	-e 's,@PREFIX@,$(OBJDIR),g'     \
	-e 's,@KCROSS@,klibc/,g'        \
	-e 's,@CFLAGS@,$(CFLAGS),g'     \
	-e 's,@LDSCRIPT@,$(LDSCRIPT),g' \
	-e 's,@LDOPTS@,$(LDOPTS),g'

PERL ?= /usr/bin/perl
#SUBDIRS := klibc

build-local: $(OBJDIR)/libc.a $(OBJDIR)/klcc $(OBJDIR)/rootfs.o

$(OBJDIR)/klibc.config:
	$(MKKLIBCCONFIG) < klcc/klibc.config > $@

$(OBJDIR)/libc.a: $(OBJDIR)/klibc.config
	$(MAKE) PRJROOT=$(PRJROOT) \
		TOOLSDIR=$(TOOLSDIR) \
		MODULE=klibc \
		DESTDIR=$(OBJDIR)/klibc \
		OBJDIR=$(OBJDIR) all install -C klibc

$(OBJDIR)/klcc: klcc/klcc.in klcc/makeklcc.pl $(OBJDIR)/klibc.config
	$(PERL)	./klcc/makeklcc.pl klcc/klcc.in $(OBJDIR)/klibc.config \
		$(PERL) > $@
	chmod u+x $@

$(OBJDIR)/rootfs.cpio:
	( cd $(MK_ROOTFS_DIR); find . | cpio -o ) > $@

$(OBJDIR)/rootfs.o: $(OBJDIR)/rootfs.cpio
	$(CC) $(CC_FLAGS) -DROOTFS_CPIO=\"$(OBJDIR)/rootfs.cpio\" -c rootfs.S -o $(OBJDIR)/rootfs.o
	echo $(OBJDIR)/rootfs.o >> $(OBJDIR)/$(OBJLIST)

clean-local:
	rm -f $(OBJDIR)/rootfs.cpio
	rm -f $(OBJDIR)/rootfs.o

.PHONY : $(OBJDIR)/rootfs.cpio $(OBJDIR)/rootfs.o

