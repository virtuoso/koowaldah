
#include <koowaldah.h>
#include <textio.h>
#include <arch/asm.h>
#include <bug.h>

void i386_display_regs(struct register_frame frame)
{
	kprintf("EAX: 0x%x EBX: 0x%x ECX: 0x%x EDX: 0x%x\n"
		"ESI: 0x%x EDI: 0x%x EBP: 0x%x ESP: 0x%x\n"
		"EIP: 0x%x CS: 0x%x EFLAGS: 0x%x\n",
		frame.eax, frame.ebx, frame.ecx, frame.edx,
		frame.esi, frame.edi, frame.ebp, frame.esp,
		frame.prev_eip, frame.prev_cs, frame.prev_eflags);
}

void i386_dump_stack(u32 * stack)
{
	hex_dump((char *) stack,
		((u32)stack | (PAGE_SIZE - 1)) - (u32) stack);
}

