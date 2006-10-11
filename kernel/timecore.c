/*
 * Basic kernel timer implementation
 * (at least I can sleep after this)
 */
#include <kuca.h>
#include <klist0.h>
#include <textio.h>
#include <irq.h>
#include <error.h>
#include <mm.h>
#include <thread.h>

static struct klist0_node timers;

typedef void (*timer_fn)(void *data);

struct timer {
	struct klist0_node t_list;
	timer_fn	func;
	u32		counter;
	void		*data;
};

#define _TIMER(p) ((struct timer *)p)

void __init timers_init()
{
	KLIST0_INIT(&timers);
}

int register_timer(timer_fn func, u32 delay, void *data)
{
	struct timer *timer;

	/* lock here, shall we? */
	disable_interrupts();

	timer = memory_alloc(sizeof(struct timer));
	if (!timer) {
		enable_interrupts();
		return ERR_NOMEMORY;
	}

	timer->func = func;
	timer->counter = delay;
	timer->data = data;

        klist0_append(&timer->t_list, &timers);

	enable_interrupts();

        return 0;
}

void update_timers()
{
        struct klist0_node *t;
	struct timer *timer;

        if (!klist0_empty(&timers)) {

		klist0_for_each(t, &timers) {
			timer = klist0_entry(t, struct timer, t_list);
			if (timer->counter)
				timer->counter--;
			else {
				timer->func(timer->data);
				klist0_unlink(t);
				memory_release(timer);
			}
                }
        }
}

static void tsleep_alarm(void *data)
{
	struct thread_t *thread = (struct thread_t *)data;

	thread->state = THREAD_RUNNABLE;
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

