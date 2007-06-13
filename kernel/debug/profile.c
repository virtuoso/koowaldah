/*
 * kernel/debug/profile.c
 *
 * Copyright (C) 2006 Alexander Shishkin
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Koowaldah developers nor the names of their
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE DEVELOPERS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE DEVELOPERS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

/*
 * Static instrumentation
 */

#include <koowaldah.h>
#include <profiler.h>
#include <textio.h>
#include <machine.h>
#include <bug.h>

/*
 * The event to be logged
 */
struct prof_event {
	void *function;
	void *call_site;
	u32 time;
	u8 type;
};

extern struct prof_trigger profiler_triggers[];
extern int NUM_TRIGGERS;

/*
 * Profiler modes
 */
enum {
	PROFILER_BOOTUP,    /* system bootup, don't log */
	PROFILER_STOPPED,   /* profiler inactive */
	PROFILER_RUNNING,   /* profiler logging */
	PROFILER_NEEDFLUSH, /* event log full, need flush */
	PROFILER_BUSY       /* doing something very important */
};

#define PROFBUF_MAX OPT_EVENTLOG_SIZE

/* event log */
static struct prof_event events[PROFBUF_MAX];
static u32 top = 0;

/* profiler operating mode */
static u32 mode = PROFILER_BOOTUP;
/* function calls during bootup */
static u32 bootup_fn = 0;

/* is the event log full? */
#define overflow() ((top < PROFBUF_MAX) ? 0 : 1)

/*
 * Never, ever use kprintf in profiler code; use this instead if you
 * absolutely have to print stuff
 */
#define npkprintf(fmt, args ...)       \
	do {                           \
		u32 __mode = mode;     \
		mode = PROFILER_BUSY;  \
		kprintf(fmt, ## args); \
		mode = __mode;         \
	} while (0);

/*
 * Dump said amount of event from the event log
 */
static __inline void __noprof dump_events(u32 t)
{
	u32 i;

	for (i = 0; i < t; i++)
		npkprintf("#KM:%d %x %s %x\n",
				events[i].time,
				events[i].call_site,
				(events[i].type == 1 ? "enters" : "leaves"),
				events[i].function);
}

/*
 * Flush the event log (dump and empty)
 */
static void __noprof flush_events()
{
	dump_events(top);
	top = 0;
}

/*
 * Collect a timer value from the time source
 * (we don't have a sane one atm, so rely on timer ticks)
 */
#define NOW() ({ \
		volatile u64 __t64 = jiffies; \
		volatile u32 __t32 = (u32)__t64; \
		__t32; \
	})

/*
 * Verify if given function's entry or exit point
 * matches some 'start logging' trigger
 * @func -- function address where we are at the moment
 * @type -- either entry or exit
 */
static __inline int __noprof start_trigger(void *func, u8 type) 
{
	int i;

	for (i = 0; i < NUM_TRIGGERS; i++)
		if (
		    profiler_triggers[i].type == type     &&
		    profiler_triggers[i].function == func &&
		    (profiler_triggers[i].flags & PROF_START)
		   ) {
			if (profiler_triggers[i].flags & PROF_DUMP)
				flush_events();
			return 1;
		}

	return 0;
}

/*
 * Verify if given function's entry or exit point
 * matches some 'stop logging' trigger
 * @func -- function address where we are at the moment
 * @type -- either entry or exit
 */
static __inline int __noprof stop_trigger(void *func, u8 type) 
{
	int i;

	for (i = 0; i < NUM_TRIGGERS; i++)
		if (
		    profiler_triggers[i].type == type     &&
		    profiler_triggers[i].function == func &&
		    (profiler_triggers[i].flags & PROF_STOP)
		   ) {
			if (profiler_triggers[i].flags & PROF_DUMP)
				flush_events();
			return 1;
		}

	return 0;
}

/*
 * Write current event to the event log
 * @this_fn   -- function address
 * @call_site -- where it was called from
 * @type      -- entry or exit
 */
static void __noprof log_event(void *this_fn, void *call_site, u8 type)
{
	if (!overflow()) {
		events[top].type = type;
		events[top].time = NOW();
		events[top].function = this_fn;
		events[top++].call_site = call_site;
		if (top == PROFBUF_MAX)
			mode = PROFILER_NEEDFLUSH;
	} else
		mode = PROFILER_NEEDFLUSH;
}

/*
 * Behave depending on current operation mode, the main part of FSM.
 */
static __inline void __noprof do_profile(void *this_fn, void *call_site, u8 type)
{
check_mode:
	switch (mode) {
		/* doing something uninterruptible;
		 * kindly apologise and return immediately */
		case PROFILER_BUSY:
			return;

		/* during bootup we only count calls */
		case PROFILER_BOOTUP:
			if (mach_state == MACH_RUNNING) {
				npkprintf("\nFunction calls during early bootup: %d\n", bootup_fn);
				mode = PROFILER_STOPPED;
				goto check_mode;
			} else
				bootup_fn += type == PROF_ENTRY;
			break;

		/* in a stopped state, check if we have corresponding
		 * start trigger */
		case PROFILER_STOPPED:
			if (start_trigger(this_fn, type)) {
				mode = PROFILER_RUNNING;
				goto check_mode;
			}
			break;

		/* the event log is full, yet we have an function call
		 * to log, thus no break here */
		case PROFILER_NEEDFLUSH:
			flush_events();

		/* in a running state the only thing we care about is
		 * a stop trigger, but we log_event() first */
		case PROFILER_RUNNING:
			log_event(this_fn, call_site, type);

			/* log_event() might have changed the mode */
			if (mode == PROFILER_NEEDFLUSH)
				flush_events();

			if (stop_trigger(this_fn, type))
				mode = PROFILER_STOPPED;
			break;
		
		default:
			break;
	}
}

/*
 * Compiler stubs, which get called upon each function entry or exit
 */
void __noprof __cyg_profile_func_enter(void *this_fn, void *call_site)
{
	do_profile(this_fn, call_site, PROF_ENTRY);
}

void __noprof __cyg_profile_func_exit(void *this_fn, void *call_site)
{
	do_profile(this_fn, call_site, PROF_EXIT);
}


