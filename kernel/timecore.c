/*
 * Basic kernel timer implementation
 * (at least I can sleep after this)
 */
#include <kuca.h>
#include <klist.h>
#include <textio.h>
#include <irq.h>
#include <error.h>
#include <mm.h>
#include <thread.h>

static struct klist *timers = NULL;

typedef void (*timer_fn)(void *data);

struct timer {
	timer_fn	func;
	u32		counter;
	void		*data;
};

#define _TIMER(p) ((struct timer *)p)

int register_timer(timer_fn func, u32 delay, void *data)
{
        struct klist *new;
	struct timer *timer;

	/* lock here, shall we? */
	disable_interrupts();

        new = klist_new();
        if (!new)
                return ERR_NOMEMORY;
	
	timer = memory_alloc(sizeof(struct timer));
	if (!timer) {
		/* XXX: kill a list entry */
		return ERR_NOMEMORY;
	}

	timer->func = func;
	timer->counter = delay;
	timer->data = data;

        new->data = timer;
        klist_add(new, &timers);

	enable_interrupts();

        return 0;
}

void update_timers()
{
        struct klist *tmp;
	struct timer *timer;

        if (!klist_is_empty(&timers)) {
                tmp = timers;

                do {
			timer = _TIMER(tmp->data);
			if (timer->counter)
				timer->counter--;
			else {
				timer->func(timer->data);
				klist_del(tmp, &timers);
				memory_release(timer);
			}
                        klist_iter(&tmp);

			/* XXX: klist needs fixing badly */
			if (!timers)
				tmp = NULL;
                } while (tmp != timers);
        }
}

static void tsleep_alarm(void *data)
{
	struct thread_t *thread = (struct thread_t *)data;

	thread->state = THREAD_RUNNABLE;
	//printf("waking up thread %d\n", thread->pid);
}

/* sleep on timer ticks */
int tsleep(u32 delay)
{
	struct thread_t *thread = CURRENT();
	int ret;

	disable_interrupts();

	/* remove the thread from scheduler's candidates */
	thread->state &= ~THREAD_RUNNABLE;
	//thread->state = THREAD_WAIT;

	/* this will wake us up after /at least/ delay ticks */
	ret = register_timer(tsleep_alarm, delay, (void *)thread);
	enable_interrupts();

	/* quit running immediately */
	if (!ret)
		thread_yield();

	return ret;
}

