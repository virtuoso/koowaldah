
/*
 * arch/i386/trap.c
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
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
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
void page_fault()
{
	u32 addr = read_cr2();
	u32 *frame;
	u32 err;
	struct thread *me = CURRENT();
	char errstr[PFLT_ERRMAX];

	if (dfault)
		panic("DOUBLE PAGE FAULT");

	dfault++;
	frame = (u32 *)me;
	frame -= 7;
	err = frame[0];
	/* try and decipher error code */
	snprintf(errstr, PFLT_ERRMAX, "%s%s%s%s",
			(err & PFLT_RESB  ? "res. bit, "    : "--//--, "     ),
			(err & PFLT_PROT  ? "protection, "  : "non-present, "),
			(err & PFLT_WRITE ? "write, "       : "read, "       ),
			(err & PFLT_USER  ? "user."         : "supervisor."  )
		);
	kprintf("### ERROR:   %x: %s\n", err, errstr);
	kprintf("### IP:      %x\n", frame[1]);
	kprintf("### ESP:     %x\n", frame[4]);
	kprintf("### ADDRESS: %x\n", addr);
	kprintf("### ESPPHYS: %x\n", __virt2physpg(frame[4]));

	if (err & PFLT_USER) {
		frame = (u32 *)frame[4];
		while ((u32)frame < USERMEM_STACK+PAGE_SIZE) {
			kprintf("%x:<%x> ", frame, *frame);
			frame++;
		}
	} else
		arch_display_stack();
	
	dfault--;
	panic("PAGE_FAULT");
}

extern void sys_call_entry();
extern void ud2_entry();
void __init trap_init(void)
{
	trapgate_init(6, ud2_entry);
	trapgate_init(13, general_protection);
	trapgate_init(14, page_fault);
	intgate_init(0x40, sys_call_entry);
}

