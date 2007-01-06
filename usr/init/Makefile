CFLAGS := -nostdlib -ffreestanding -I../libc/include -g

all: init

init: init.c
	$(CC) $(CFLAGS) -o init init.c -L../libc -lc -static -Wl,-T,../libc/sys/i386/kos-aout.lds,--omagic

clean:
	rm -f init

.PHONY: all clean
