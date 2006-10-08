/*
 * KucaOS kernel threads implementation
 */

#include <kuca.h>
#include <mm.h>
#include <textio.h>
#include <thread.h>
#include <bug.h>

/* each architecture should define this */
extern void thread_init_stack(struct thread_t * t, void (*func)(void));

/* omgwtf */
u32 get_free_pid()
{
	static u32 i = 1;
	return i++;
}

struct thread_t * thread_create(void (*func)())
{
	void *page;
	struct thread_t * thread;
	
	printf("thread_create, func = %x\n", func);

	/* allocate stack space */
	page = pages_get(THREAD_STACK_LIMIT/PAGE_SIZE);
	if (!page) {
		printf("Thread allocation screwed up. Don't panic!\n");
		return NULL;
	}

	/* now, place the task descriptor */
	thread = THREAD(page);
	thread->state = THREAD_NEW;
	printf("page=%x, thread=%x\n", page, thread);

	/* stack pointers */
	tctx(thread).stack_base = (u32 *)thread - 1;
	tctx(thread).esp = tctx(thread).stack_base;
	thread_init_stack(thread, func);

	thread->pid = get_free_pid();

	/* We place a reference to the thread_t on the bottom of the stack */ 

	printf("created thread, stack_base = %x, esp = %x, pid = %d\n", 
			(u32) (tctx(thread).stack_base),
			(u32) (tctx(thread).esp), thread->pid);

	return thread;
}

