include $(PRJROOT)/tools/main.mk

build-local: $(OBJDIR)/rootfs.o

$(OBJDIR)/rootfs.cpio:
	( cd $(MK_ROOTFS_DIR); find . | cpio -o ) > $@

$(OBJDIR)/rootfs.o: $(OBJDIR)/rootfs.cpio
	$(CC) $(CC_FLAGS) -DROOTFS_CPIO=\"$(OBJDIR)/rootfs.cpio\" -c rootfs.S -o $(OBJDIR)/rootfs.o
	echo $(OBJDIR)/rootfs.o >> $(OBJDIR)/OBJECTS

clean-local:
	rm -f $(OBJDIR)/rootfs.cpio
	rm -f $(OBJDIR)/rootfs.o

.PHONY : $(OBJDIR)/rootfs.cpio $(OBJDIR)/rootfs.o

