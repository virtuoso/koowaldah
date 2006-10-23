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

rootfs.cpio: init/init
	mkdir -p rootfs/sbin
	cp init/init rootfs/sbin/init
	( cd rootfs; find . | cpio -o ) > $@

