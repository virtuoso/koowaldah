
#include <koowaldah.h>
#include <textio.h>
#include <i386/pages.h>
#include <bug.h>
#include <thread.h>
#include <symtab.h>

void i386_display_regs(struct register_frame frame)
{
	kprintf("EAX: 0x%08x EBX: 0x%08x ECX: 0x%08x EDX: 0x%08x\n"
		"ESI: 0x%08x EDI: 0x%08x EBP: 0x%08x ESP: 0x%08x\n"
		"EIP: 0x%08x CS:  0x%08x EFLAGS: 0x%08x\n",
		frame.eax, frame.ebx, frame.ecx, frame.edx,
		frame.esi, frame.edi, frame.ebp, frame.esp,
		frame.prev_eip, frame.prev_cs, frame.prev_eflags);
}

void i386_dump_stack(u32 *stack)
{
	u32 *stop = (u32 *)THREAD(stack);
	u32 *frame;
	int sym;

	kprintf("\nCall trace: [%08x]\n", stop);
	/* unwinding from bottom to top */
	for (frame = stack; frame <= stop; frame++) {
		/*
		 * It should be sufficient to find a pair:
		 * <address-somewhere-in-text>   ! produced by call
		 * <address-HIGHER-on-the-stack> ! produced by push %ebp
		 * The latter is (in theory) a must with gcc's
		 * -fno-omit-frame-pointer.
		 * "HIGHER" stands for "no lower than current stack ptr".
		 */
		u32 *next = frame - 1;

		/* check if <next> points to our current stack */
		if ((*next & NOPAGE_MASK) != ((u32)frame & NOPAGE_MASK))
			continue;

		/* this will ensure we're somewhere in .text */
		sym = symtab_lookup_addr(*frame);

		if (sym && (*next >= (u32)frame)) {
			kprintf("[%08x] %08x: %s\n", frame, *frame,
					symbol_table[sym].name);
			frame = (u32 *)*next;
		}
	}
	kprintf("\n");
	hex_dump((char *)stack,
		((u32)stop) - (u32)stack);
}


