CFLAGS := -ffreestanding -Iinclude

OBJS := \
	sys/i386/syscall.S.o \
	sys/i386/syscall.o

%.S.o:
	$(AS) -o $@ $(subst .o,,$@)

%.o:
	$(CC) $(CFLAGS) -o $@ -c $(subst .o,.c,$@)

all: libc.a

libc.a: $(OBJS)
	ar cru $@ $(OBJS)

clean:
	rm -f $(OBJS) libc.a

.PHONY: all clean
