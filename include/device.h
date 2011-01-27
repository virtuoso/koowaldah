/*
 * Copyright (c) 2006, Alexander Shishkin
 * All rights reserved.
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
 */

/*
 * Device related types, constants and helpers
 */
#ifndef __DEVICE_H__
#define __DEVICE_H__

#define NODEV (0x0)
#define ROOTFSDEV (0x1)
#define VIRTFSDEV (0x2)

typedef u32 dev_t;

#define DEV_MAJOR_BITS (16)
#define DEV_MINOR_BITS (sizeof(dev_t) * 8 - DEV_MAJOR_BITS)

/*
 * Obtain major/minor number of a device
 * @d -- the device
 */
#define DEV_MAJOR(d) (((d) >> DEV_MAJOR_BITS) & ((1 << DEV_MAJOR_BITS) - 1))
#define DEV_MINOR(d) ((d) & ((1 << DEV_MINOR_BITS) - 1))

/*
 * Construct a device number from major and minor
 * @maj -- major number
 * @min -- minor number
 */
#define DEV_DEVICE(maj, min) (((maj) << DEV_MAJOR_BITS) | (min))

#define DEVNAME_MAX 32

/*
 * device flags
 */
#define DF_RDONLY 0x1
#define DF_WRONLY 0x2
#define DF_CHAR   0x4

struct device {
	char d_name[DEVNAME_MAX];
	dev_t d_dev;
	u32 d_flags;
	struct file_operations *d_fops;
	struct klist0_node d_list;
};

int register_device(struct device *dev);
struct device *get_device(dev_t dev);
struct device *get_device_by_name(char *name);
void __init devices_init();

#endif
