/*
 * kernel/fs/device.c
 *
 * Copyright (C) 2006 Alexander Shishkin
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Koowaldah developers nor the names of their
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE DEVELOPERS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE DEVELOPERS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

#include <koowaldah.h>
#include <super.h>
#include <inode.h>
#include <namespace.h>
#include <textio.h>
#include <klist0.h>
#include <lib.h>
#include <error.h>

static struct klist0_node devices;

/*
 * Register a new device within the device list
 * @device -- pointer to device descrition structure
 */
int register_device(struct device *device)
{
	klist0_append(&device->d_list, &devices);
	return 0;
}

/*
 * Find a device by a given device number
 * @dev -- device number
 */
struct device *get_device(dev_t dev)
{
	struct klist0_node *t;
	struct device *device;

	klist0_for_each(t, &devices) {
		device = klist0_entry(t, struct device, d_list);
		if (device->d_dev == dev)
			return device;
	}

	return NULL;
}

/*
 * Find a device by name
 * @name -- device name to look for
 */
struct device *get_device_by_name(char *name)
{
	struct klist0_node *t;
	struct device *device;

	klist0_for_each(t, &devices) {
		device = klist0_entry(t, struct device, d_list);
		if (!kstrncmp(device->d_name, name, DEVNAME_MAX))
			return device;
	}

	return NULL;
}

void __init devices_init()
{
	KLIST0_INIT(&devices);
}

