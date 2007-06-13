-include ../konfig.mk

MK_ROOTFS_DIR ?= ./rootfs
ifeq ($(MK_LOCAL_LIBC),Y)
  rootfs_target := init/init
endif

all: rootfs.o
clean:
	-rm -f rootfs.cpio rootfs.o
	$(MAKE) clean -C init
	$(MAKE) clean -C libc

rootfs.o: rootfs.cpio
	$(AS) rootfs.S -o $@

libc/libc.a:
	$(MAKE) all -C libc

init/init: libc/libc.a
	$(MAKE) all -C init

rootfs.cpio: $(rootfs_target)
ifeq ($(MK_LOCAL_LIBC),Y)
	mkdir -p $(MK_ROOTFS_DIR)/sbin
	cp init/init $(MK_ROOTFS_DIR)/sbin/init
endif
	( cd $(MK_ROOTFS_DIR); find . | cpio -o ) > $@

