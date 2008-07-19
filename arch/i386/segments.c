/*
 * arch/i386/segments.c
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

#include <koowaldah.h>
#include <i386/segments.h>
#include <i386/asm.h>
#include <textio.h>

extern struct segdesc  gdt_table;
extern struct gatedesc idt_table;

struct tss_segment root_tss;

/*
 * Initialize a code or data segment descriptor in GDT
 * @idx  -- descriptor index
 * @base -- base address
 * @size -- size in pages (granularity bit is always set)
 * @type -- segment type (see ST_* constants)
 * @priv -- privilege level (see DPL_* constants)
 * For static definition, use SEGDESC() macro.
 */
void segdesc_init(u32 idx, u32 base, u32 size, u8 type, u8 priv)
{
	struct segdesc *s = &gdt_table;
	int n = idx >> 3;

	/* size and base */
	s[n].size = (u16)size;
	s[n].attr = (u16)((size & 0x0f0000) >> 8);
	s[n].base_low = (u16)base;
	s[n].base_mid = (u8)(base >> SD_BASEMID_SHIFT);
	s[n].base_high = (u8)(base >> SD_BASEHIGH_SHIFT);

	/* attrs */
	s[n].attr |= SD_ATTRS(type, priv);
}

/*
 * Initialize a system segment or gate descriptor in GDT
 * @idx  -- descriptor index
 * @base -- base address
 * @size -- size in pages (granularity bit is always set)
 * @type -- segment type (see ST_* constants)
 * @priv -- privilege level (see DPL_* constants)
 * For static definition, use SEGDESC() macro.
 */
void syssegdesc_init(u32 idx, u32 base, u32 size, u8 type, u8 priv)
{
	struct segdesc *s = &gdt_table;
	int n = idx >> 3;

	/* size and base */
	s[n].size = (u16)size;
	s[n].attr = (u16)((size & 0x0f0000) >> 8);
	s[n].base_low = (u16)base;
	s[n].base_mid = (u8)(base >> SD_BASEMID_SHIFT);
	s[n].base_high = (u8)(base >> SD_BASEHIGH_SHIFT);

	/* attrs */
	s[n].attr |= SD_SYSATTRS(type, priv);
}

/*
 * Initialize a gate descriptor in IDT
 * @idx    -- descriptor index
 * @offset -- handler offset
 * @type   -- gate type
 * @priv   -- gate privilege level
 */
void gatedesc_init(u32 idx, u32 offset, u8 type, u8 priv)
{
	struct gatedesc *g = &idt_table;
	int n = idx >> 3;

	/* selector */
	g[n].selector = KERN_CODE;
	/* offset */
	g[n].off_low = (u16)offset;
	g[n].off_high = (u16)(offset >> 16);

	/* attrs */
	g[n].attr = GD_ATTRS(type, priv);
}

void __init init_root_tss()
{
	root_tss.ss0 = KERN_DATA;

	root_tss.iobase = offsetof(struct tss_segment, iomap);
	root_tss.iomap[IOPERMSZ - 1] = 0xff;

	syssegdesc_init(TSS, (u32)&root_tss, sizeof(struct tss_segment),
			GT_FREE_TSS, DPL_KERN);
	write_tr(TSS);
}

/*
 * Dump GDT descriptors
 */
void gdt_dump()
{
	struct segdesc *s = &gdt_table;
	int i;

	for (i = 0; i < GDT_ENTRIES; i++)
		kprintf(
			"## segment %d:\n"
			"#  size=%x\n"
			"#  base=%x\n"
			"#  attributes: %x\n",
			i, s->size | (s->attr & 0x0f00) << 8,
			s[i].base_low | (s[i].base_mid << 16) | (s[i].base_high << 24),
			s[i].attr
		);
}

/*
 * Dump IDT descriptors
 */
void idt_dump()
{
	struct gatedesc *g = &idt_table;
	int i;

	for (i = 0; i < IDT_ENTRIES; i++)
		kprintf(
			"## gate %d:\n"
			"#  handler=%x:%x\n"
			"#  attributes: %x\n",
			i,
			g[i].selector, g[i].off_low | (g[i].off_high << 16),
			g[i].attr
		);
}

