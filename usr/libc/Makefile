CFLAGS := -g -ffreestanding -nostdinc -nostdlib -Iinclude

OBJS := \
	string/strlen.o \
	string/strcmp.o \
	string/strchr.o \
	string/memccpy.o \
	string/memset.o \
	string/strcoll.o \
	string/strcspn.o \
	string/strmode.o \
	string/strncat.o \
	string/strpbrk.o \
	string/strsep.o \
	string/strspn.o \
	string/strtok.o \
	string/strtok_r.o \
	string/strxfrm.o \
	string/swab.o \
	stdio/vsnprintf.o \
	sys/i386/syscall.S.o \
	sys/i386/syscall.o

%.S.o:
	$(AS) -o $@ $(subst .o,,$@)

%.o:
	$(CC) $(CFLAGS) -o $@ -c $(subst .o,.c,$@)

all: libc.a

libc.a: $(OBJS)
	$(AR) cru $@ $(OBJS)

clean:
	rm -f $(OBJS) libc.a

.PHONY: all clean
