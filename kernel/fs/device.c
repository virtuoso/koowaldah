/*
 * kernel/fs/device.c
 *
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
#include <super.h>
#include <inode.h>
#include <namespace.h>
#include <textio.h>
#include <klist0.h>
#include <lib.h>
#include <error.h>

struct klist0_node devices;

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

