#include <syscalls.h>

char buf[16];
void _start()
{
	int fd, l;
	unsigned short c;

	for (l = 0; l < 16; l++) buf[l] = '\0';
	sys_debug("Init process starting!");
	fd = sys_open("/init", 0);
	l = sys_read(fd, buf, 12);
	sys_debug(buf);

	for (;;) {
		sys_debug("waking up");
		for (fd = 0; fd < 0x8000000; fd++);
	}
}

