/*
 * KucaOS kernel threads implementation
 */

#include <kuca.h>
#include <mm.h>
#include <textio.h>
#include <thread.h>
#include <bug.h>
#include <klist0.h>

/* each architecture should define this */
extern void thread_init_stack(struct thread_t * t, void (*func)(void));

/* omgwtf */
u32 get_free_pid()
{
	static u32 i = 1;
	return i++;
}

void dump_thread(struct thread_t *thread)
{
	printf("Thread %x \"%s\":\n"
		"\tpid: %d; stack: [%x..%x]\n"
		"\tstate: %s\n"
		"\tneighbours: %x, %x\n",
		thread, thread->name, thread->pid,
		tctx(thread).esp,
		tctx(thread).stack_base,
		(thread->state & THREAD_RUNNABLE ? "runnable" : "waiting"),
		thread->kthreads.prev, thread->kthreads.next
	);
}

void init_thread_struct(struct thread_t *thread)
{
	/* stack pointers */
	tctx(thread).stack_base = (u32 *)thread - 1;
	tctx(thread).esp = tctx(thread).stack_base;

	thread->pid = 0;
	thread->state = THREAD_RUNNABLE;
	KLIST0_INIT(&thread->kthreads);
}

extern struct klist0_node thread_list;
struct thread_t * thread_create(void (*func)(), char *name)
{
	void *page;
	struct thread_t * thread;
	
	printf("thread_create, func = %x, name: %s\n", func, name);

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
	thread->state = THREAD_RUNNABLE;
	memory_copy(thread->name, name, MIN(string_len(name), THREAD_NAME_LEN));
	KLIST0_INIT(&thread->kthreads);
	KLIST0_INIT(&thread->krunq);
	klist0_append(&thread->kthreads, &thread_list);

	/* We place a reference to the thread_t on the bottom of the stack */ 

	printf("created thread, stack_base = %x, esp = %x, pid = %d\n", 
			(u32) (tctx(thread).stack_base),
			(u32) (tctx(thread).esp), thread->pid);

	return thread;
}

void thread_yield()
{
	thread_reschedule();
}

