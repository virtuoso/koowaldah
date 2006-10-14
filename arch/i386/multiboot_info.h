#ifndef __MULTIBOOT_INFO_H__
#define __MULTIBOOT_INFO_H__

/* Stuff, passed to us by the multiboot loader. */

#include <kuca.h>

#define INFO_MEM 1<<0
#define INFO_BOOTDEV 1<<1
#define INFO_CMDLINE 1<<2
#define INFO_MODULES 1<<3
#define INFO_SYMS_AOUT 1<<4
#define INFO_SYMS_ELF 1<<5
#define INFO_MMAP 1<<6


#define REGION_RAM 1 /* Anything else is reserved. */

struct multiboot_mem_region {
	u32 size;
	u32 base_addr_low;
	u32 base_addr_high;
	u32 length_low;
	u32 lenght_high;
	u32 type;
};

struct multiboot_info_t {
	u32 flags;
	u32 mem_lower;
	u32 mem_upper;
	char boot_device[4];
	char * cmdline;
	u32 modules_count;
	u32 modules_address;
	u32 syms[4];
	u32 mmap_length;
	struct multiboot_mmap * mmap_address;
	u32 not_needed[11];
};


#endif /* __MULTIBOOT_INFO_H__ */

