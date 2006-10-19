all: rootfs.o
clean:
	-rm -f rootfs.cpio rootfs.o

rootfs.o: rootfs.cpio
	$(AS) rootfs.S -o $@

rootfs.cpio:
	( cd rootfs; find . | cpio -o ) > $@

