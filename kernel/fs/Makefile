SOURCES := \
	super.c \
	inode.c \
	file.c \
	stat.c \
	namespace.c \
	message.c \
	mount.c \
	fs.c \
	device.c \
	mpipe.c \
	virtfs.c \
	virtfs_proc.c \
	virtfs_dev.c \
	cpio.c

-include $(PRJROOT)/konfig.mk
ifeq ($(MK_AOUT_SUPPORT),Y)
SOURCES += aout.c
endif

include $(PRJROOT)/tools/main.mk

