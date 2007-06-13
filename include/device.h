/*
 * Copyright (c) 2006, Alexander Shishkin
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the University of California, Berkeley nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE REGENTS AND CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
