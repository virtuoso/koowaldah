
#include <koowaldah.h>
#include <textio.h>
#include <arch/asm.h>
#include <bug.h>

void i386_display_regs(u32 *regs_dump)
{
	kprintf("EAX: 0x%x EBX: 0x%x ECX: 0x%x EDX: 0x%x\n"
		"ESI: 0x%x EDI: 0x%x EBP: 0x%x ESP: 0x%x\n",
		regs_dump[0], regs_dump[1], regs_dump[2], regs_dump[3],
		regs_dump[4], regs_dump[5], regs_dump[6], regs_dump[7]);
}

void i386_dump_stack(u32 * stack)
{
	hex_dump((char *) stack,
		((u32)stack | (PAGE_SIZE - 1)) - (u32) stack);
}

