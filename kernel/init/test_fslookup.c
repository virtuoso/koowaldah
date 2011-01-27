/*
 * kernel/init/test_fslookup.c
 *
 * Copyright (C) 2006 Alexey Zaytsev
 * Copyright (C) 2006 Alexander Shishkin
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 * 
 */
#include <koowaldah.h>
#include <bug.h>
#include <textio.h>
#include <namespace.h>
#include <lib.h>

void test_fslookup()
{
#ifdef OPT_TEST_FSLOOKUP
	struct direntry *dent = lookup_path("/sbin/init");
	int fd, l;
	char buf[256];
	
	if (!dent)
		bug();

	kprintf("FS test found: %s\n", dent->d_name);

	fd = open("/init", 0);
	if (fd < 0) {
		kprintf("Open failed! %d\n", fd);
		return;
	}
	kprintf("Opened a file descriptor %d\n", fd);

	memory_set(buf, 0, 256);
	do {
		l = read(fd, buf, 100);
		if (l < 0) {
			kprintf("Error %d occured\n", l);
			return;
		}
		kprintf("Read %d bytes, [%s]\n", l, buf);
		break;
	} while (l == 100);
#endif
}
