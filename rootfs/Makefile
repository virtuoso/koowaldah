# things supported by mkbimage (of grub): 1.20, 1.44, 1.60, 1.68, 1.74, 2.88, hd
IMG_TYPE ?= 1.44
IMG_NAME := $(IMG_TYPE).image

# We only need root if we don't have e2tools installed.
ifeq (,$(shell which e2cp))
	ROOTCMD ?= sudo
endif

MKBIMAGE = $(ROOTCMD) /bin/mkbimage

all: rootfs.img
	rm -f rootfs.tar

clean:
	-rm -f *.image device.map rootfs.tar rootfs.img
	-rm -fr *.image1

rootfs.img: $(IMG_NAME)
	mv $< $@

$(IMG_NAME): rootfs.tar
	$(MKBIMAGE) -t $(IMG_TYPE) -f $<

rootfs.tar: forge
	cp -a ../kos-elf forge/boot
	strip forge/boot/kos-elf
	( cd forge; tar cf ../rootfs.tar ./* )

