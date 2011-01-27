
/*
 * arch/i386/trap.c
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
 * Ok, now we can generate exceptions, so let's fucking handle 'em right.
 */

#include <koowaldah.h>
#include <textio.h>
#include <lib.h>
#include <bug.h>
#include <thread.h>
#include <mem_area.h>
#include <page_alloc.h>
#include <i386/segments.h>
#include <arch/asm.h>


u32 fault_code;

void ud2_handler(struct register_frame frame)
{
	u32 eip;
	u16 line;
	char *file;

	/*
	 * XXX check that the instruction was really ud2 and that it was
	 * issued from the kernel space. 
	 */

	eip = frame.prev_eip;
	
	/* eip now points at the ud2 instruction. */
	
	eip += 2; /* Skip the ud2 instruction. */
	
	line = (u16) *(u16 *)eip;

	eip += 2; /* Skip the line number. */

	
	file = (char *) *(char **)eip;
	
	kprintf("Bug in file %s, line %d\n", file, line);
	display_thread();
	kprintf("Registers:\n");
	i386_display_regs(frame);
	kprintf("Stack:\n");
	
	i386_dump_stack((u32 *)frame.esp);
	for(;;) {
		kprintf("R.I.P.\n");
		arch_halt();
	}
}

void general_protection()
{
	u32 *frame;

	__asm__ __volatile__("mov %%esp, %%eax\nmov %%eax, %0" : "=m"(frame));
	kprintf("### EIP=%x\n", frame[16]);
	panic("GENERAL PROTECTION FAULT");
}

static int dfault = 0;

#define PFLT_ERRMAX 48
void pf_handler(struct register_frame frame)
{
	u32 addr = read_cr2();
	char errstr[PFLT_ERRMAX];

	if (dfault)
		panic("DOUBLE PAGE FAULT");

	dfault++;

	if (fault_code & PFLT_USER) {
		struct thread *t;
		struct mapping *map;
		int dist, i;

		/* chance is that we need to increase stack a bit */
		t = CURRENT();
		map = t->map;
		DPRINT("# address causing fault: %x\n", addr);
		for (i = 0; i < map->m_nmma; i++) {
			DPRINT("# mma[%d]: %x..%x +/- %x\n", i,
					map->m_mma[i]->m_start,
					map->m_mma[i]->m_end,
					map->m_mma[i]->m_sizelim);

			if (!mem_area_is_hit(map->m_mma[i], addr)) {
				dist = mem_area_hit_offset(map->m_mma[i], addr);
				if (dist != -1) {
					if(!dist)
						dist += PAGE_SIZE;
					DPRINT("The process hit the mma[%d]:\n", i);
					DPRINT(" * dist=%x, adding %d pages\n",
						dist, (dist + PAGE_MASK) >> PAGE_SHIFT);

					/* now fix up quickly, before anybody sees */
					mem_area_grow(map->m_mma[i],
							(dist + PAGE_MASK)
							>> PAGE_SHIFT);
					dfault--;
					return;
				}
			}
		}

		/* couldn't recover */
		kprintf("PAGE FAULT in user mode\n");
		display_map(map);
	} else {
		kprintf("PAGE FAULT in supervisor mode\n");
		arch_display_stack();
	}

	/* try and decipher error code */
	snprintf(errstr, PFLT_ERRMAX, "%s%s%s%s",
		(fault_code & PFLT_RESB  ? "res. bit, "    : "--//--, "     ),
		(fault_code & PFLT_PROT  ? "protection, "  : "non-present, "),
		(fault_code & PFLT_WRITE ? "write, "       : "read, "       ),
		(fault_code & PFLT_USER  ? "user."         : "supervisor."  )
		);
	kprintf("### ERROR:   0x%x: %s\n", fault_code, errstr);
	kprintf("### IP:      0x%x\n", frame.prev_eip);
	kprintf("### ESP:     0x%x\n", frame.esp);
	kprintf("### ADDRESS: 0x%x\n", addr);
	kprintf("### ESPPHYS: 0x%x\n", __virt2physpg(frame.prev_esp));
	kprintf("### ESPVIRT: 0x%x\n", frame.prev_esp);

	display_thread();
	kprintf("Registers:\n");
	i386_display_regs(frame);

	dfault--;

	for (;;) {
		kprintf("R.I.P\n");
		arch_halt();
	}
}

extern void sys_call_entry();
extern void ud2_entry();
extern void pf_entry();
void __init trap_init(void)
{
	trapgate_init(6, ud2_entry);
	trapgate_init(13, general_protection);
	trapgate_init(14, pf_entry);
	intgate_init(0x40, sys_call_entry);
}

