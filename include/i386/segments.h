/*
 * include/i386/segments.h
 *
 * Copyright (C) 2006 Alexander Shishkin
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Koowaldah developers nor the names of their
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
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
 */

#ifndef __I386_SEGMENTS_H__
#define __I386_SEGMENTS_H__

/*
 * segment descriptors related 
 */
#define KERN_CODE   0x08
#define KERN_DATA   0x10
#define USER_CODE   0x18
#define USER_DATA   0x20
#define TSS         0x28
#define CALL_GATE   0x30
#define GDT_ENTRIES 7 /* add null entry */

#define IDT_ENTRIES 256

/*
 * GDT entry: segment descriptor
 */
#ifndef __ASSEMBLY__
struct segdesc {
	u16 size;
	u16 base_low;
	u8  base_mid;
	u16 attr;
	u8  base_high;
} __attribute__((packed));
#endif

/* descriptor base address */
#define SD_BASELOW_BITS   16
#define SD_BASEMID_BITS   8
#define SD_BASEHIGH_BITS  8
#define SD_BASELOW_SHIFT  0
#define SD_BASEMID_SHIFT  16
#define SD_BASEHIGH_SHIFT 24

/* helpers */
#define SEGDESC_BASELOW(base)                                             \
	(                                                                 \
	 ((base) & (((1 << SD_BASELOW_BITS) - 1) << SD_BASELOW_SHIFT)) >> \
	  SD_BASELOW_SHIFT                                                \
	)

#define SEGDESC_BASEMID(base)                                             \
	(                                                                 \
	 ((base) & (((1 << SD_BASEMID_BITS) - 1) << SD_BASEMID_SHIFT)) >> \
	  SD_BASEMID_SHIFT                                                \
	)

#define SEGDESC_BASEHIGH(base)                                              \
	(                                                                   \
	 ((base) & (((1 << SD_BASEHIGH_BITS) - 1) << SD_BASEHIGH_SHIFT)) >> \
	  SD_BASEHIGH_SHIFT                                                 \
	)

/*
 * Attribute-related constants
 */

/* access (lower byte) */
/* - set by cpu if segment had been accessed */
#define SD_ACCESS_MASK   0x0001
#define SD_ACCESS_SHIFT  0
/* - segment type */
#define SD_TYPE_MASK     0x000e
#define SD_TYPE_SHIFT    1
/* - reserved set bit */
#define SD_SYSTEM        0x0010
/* - segment presence */
#define SD_PRESENT_MASK  0x0080
#define SD_PRESENT_SHIFT 7
/* - descriptor privilege level */
#define SD_DPL_MASK      0x0060
#define SD_DPL_SHIFT     5

/* higher byte */
/* - high 8 bits of segment size */
#define SD_SIZE_MASK     0x0f00
#define SD_SIZE_SHIFT    8
/* - availability */
#define SD_AVL_MASK      0x1000
#define SD_AVL_SHIFT     12
/* - segment is 32bit by default */
#define SD_DFL32_MASK    0x4000
#define SD_DFL32_SHIFT   14
/* - granularity: size is measured in pages if set, in bytes otherwise */
#define SD_GRAN_MASK     0x8000
#define SD_GRAN_SHIFT    15

/* segment descriptor attributes definition helper */
/*
 * set code or data segment attributes
 * @type -- code or data segment type (see ST_* constants)
 * @priv -- descriptor privilege level
 */
#define SD_ATTRS(type, priv)                \
	(SD_SYSTEM                        | \
		((type) << SD_TYPE_SHIFT) | \
		((priv) << SD_DPL_SHIFT)  | \
		SD_PRESENT_MASK           | \
		SD_DFL32_MASK             | \
		SD_GRAN_MASK                \
	)

/*
 * set system-segment or gate descriptor attributes
 * @type -- descriptor type (see GT_* constants)
 * @priv -- descriptor privilege level
 */
#define SD_SYSATTRS(type, priv)             \
	(                                   \
		((type) << 0)             | \
		((priv) << SD_DPL_SHIFT)  | \
		SD_PRESENT_MASK           | \
		SD_DFL32_MASK               \
	)

/* same, but also includes higher part of segment size */
#define SD_ATTRS_SZ(type, priv, size) \
	(                             \
	  SD_ATTRS(type, priv) |      \
	  (((size) & 0x0f0000) >> 8)  \
	)

#define SD_SYSATTRS_SZ(type, priv, size) \
	(                                \
	  SD_SYSATTRS(type, priv) |      \
	  (((size) & 0x0f0000) >> 8)     \
	)

#ifdef __ASSEMBLY__
/* segment descriptor definition helper */
#define SEGDESC(base, size, type, priv)      \
	.word (size & 0xffff);               \
	.word SEGDESC_BASELOW(base);         \
	.byte SEGDESC_BASEMID(base);         \
	.word SD_ATTRS_SZ(type, priv, size); \
	.byte SEGDESC_BASEHIGH(base)

/* either first descriptor, or to be defined at runtime */
#define SEGDESC_EMPTY \
	.quad 0x0000000000000000
#endif

/*
 * segment type:
 *  RONLY = "read only"
 *  RW = "read/write"
 *  XONLY = "execute only"
 *  RX = "read/execute"
 */
#define ST_RONLY_DATA       0
#define ST_RW_DATA          1
#define ST_RONLY_STACK      2
#define ST_RW_STACK         3
#define ST_XONLY_CODE       4
#define ST_RX_CODE          5
#define ST_XONLY_CONFORMING 6
#define ST_RX_CONFORMING    7

/*
 * segment DPL
 */
#define DPL_KERN 0x0
#define DPL_USER 0x3

/*
 * GDT entry: gate descriptor
 */
#ifndef __ASSEMBLY__
struct gatedesc {
	u16 off_low;
	u16 selector;
	u8  reserved;
	u8  attr;
	u16 off_high;
} __attribute__((packed));
#endif

/* descriptor fields */
#define GD_OFFLOW_BITS   16
#define GD_SELECTOR_BITS 16
#define GD_SYSTEM_BITS   8
#define GD_ATTR_BITS     8
#define GD_OFFHIGH_BITS  16

/* helpers */
#define GATEDESC_OFFLOW(off)                                             \
	(                                                                \
	 ((off) & (((1 << GD_OFFLOW_BITS) - 1) << GD_OFFLOW_SHIFT)) >>   \
	  GD_OFFLOW_SHIFT                                                \
	)

#define GATEDESC_OFFHIGH(off)                                            \
	(                                                                \
	 ((off) & (((1 << GD_OFFHIGH_BITS) - 1) << GD_OFFHIGH_SHIFT)) >> \
	  GD_OFFHIGH_SHIFT                                               \
	)

/*
 * Attribute-related constants
 * Gate descriptors use only one byte for attributes
 */

/* attributes */
/* - gate type (see below) */
#define GD_TYPE_MASK     0x0f
#define GD_TYPE_SHIFT    0
/* - gate privilege level */
#define GD_DPL_MASK      0x60
#define GD_DPL_SHIFT     5
/* - presence */
#define GD_PRESENT_MASK  0x80
#define GD_PRESENT_SHIFT 7

/* gate types (only those relevant to i386 and higher are used */
#define GT_LDT      0x02     /* local descriptor table  */
#define GT_TASK     0x05     /* task gate               */
#define GT_FREE_TSS 0x09     /* free task state segment */
#define GT_BUSY_TSS 0x0b     /* busy task state segment */
#define GT_CALL     0x0c     /* call gate               */
#define GT_INT      0x0e     /* interrupt gate          */
#define GT_TRAP     0x0f     /* trap gate               */

/* 
 * gate descriptor attributes definition helper
 * @type -- gate descriptor type
 * @priv -- descriptor privilege level
 */
#define GD_ATTRS(type, priv)                \
	(                                   \
		((type) << 0)             | \
		((priv) << GD_DPL_SHIFT)  | \
		GD_PRESENT_MASK             \
	)

#ifdef __ASSEMBLY__
/* gate descriptor definition helper */
#define GATEDESC(off, type, priv)           \
	.word GATEDESC_OFFLOW(off);         \
	.word $(KERN_CODE);                 \
	.byte $0x0                          \
	.byte GD_ATTRS(type, priv);         \
	.word GATEDESC_OFFHIGH(off)
#endif

/*
 * TSS segment
 */
#ifndef __ASSEMBLY__
struct tss_segment {
	u32 backlink;
	u32 esp0;
	u32 ss0;
	u32 esp1;
	u32 ss1;
	u32 esp2;
	u32 ss2;
	u32 cr3;
	u32 ip;
	u32 flags;
	u32 eax;
	u32 ecx;
	u32 edx;
	u32 ebx;
	u32 esp;
	u32 ebp;
	u32 esi;
	u32 edi;
	u32 es;
	u32 cs;
	u32 ss;
	u32 ds;
	u32 fs;
	u32 gs;
	u32 ldt;
	u32 trap;
	u32 iobase;
/* u8_t iomap[0]; */
};
#endif

/*
 * Functions for run-time tweaking descriptor tables, see segments.c
 * @idx    -- descriptor index
 * @offset -- handler offset (system segments or gates)
 * @base   -- base address
 * @size   -- size in pages (granularity bit is always set)
 * @type   -- segment type (see ST_* constants)
 * @priv   -- privilege level (see DPL_* constants)
 */
#ifndef __ASSEMBLY__
void segdesc_init(u32 idx, u32 base, u32 size, u8 type, u8 priv);
void syssegdesc_init(u32 idx, u32 base, u32 size, u8 type, u8 priv);
void gatedesc_init(u32 idx, u32 offset, u8 type, u8 priv);

#define intgate_init(num, handler) \
	gatedesc_init(num << 3, (u32)handler, GT_INT, DPL_USER);

#define sysgate_init(num, handler) \
	gatedesc_init(num << 3, (u32)handler, GT_TRAP, DPL_USER);

#define trapgate_init(num, handler) \
	gatedesc_init(num << 3, (u32)handler, GT_TRAP, DPL_KERN);
#endif

#endif /* __I386_SEGMENTS_H__ */

