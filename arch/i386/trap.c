/*
 * Ok, now we can generate exceptions, so let's fucking handle 'em right.
 */

#include <kuca.h>
#include <textio.h>
#include <irq.h>
#include <console.h>
#include <bug.h>
#include <mm.h>
#include <thread.h>

/* since we're KucaOS, our screen-of-death will be pink */
/* Pink Screen Of Death, KYCA KYKY */
void psod()
{
	u32 frame;
	struct thread_t *thread;
	int i;

	for (i = 0; i < 80; i++) console_put_char('-');

	__asm__ __volatile__("mov %%esp, %0" : "=m"(frame));
	thread = CURRENT();

	printf("Task %d, stack base: %x, esp: %x\n",
			thread->pid,
			tctx(thread).stack_base,
			tctx(thread).esp);

	frame = tctx(thread).esp;
	printf("Stack length: %d dwords\n", ((u32)thread - frame)/4);
	i = 0;
	//while ((frame + i*4) < (u32)thread) {
	for (i = 0; i < ((u32)thread - frame)/4; i++) {
		printf(" (%d) %x", i, (u32)((u32 *)frame)[i]);
		if (i && !(i % 8)) printf("\n");
	}
}

void general_protection()
{
	vga_setcolor(0xd0);
	printf("\nGENERAL PROTECTION FAULT\n");
	psod();
	for (;;) __asm__ __volatile__("hlt; nop");
}

void page_fault()
{
	vga_setcolor(0x90);
	printf("\nPAGE FAULT\n");
	psod();
	for (;;) __asm__ __volatile__("hlt; nop");
}

void __init trap_init(void)
{
//	set_trap_handler(13, general_protection);
}

