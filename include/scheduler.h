
/*
 * include/scheduler.h
 *
 * Copyright (C) 2005 Alexey Zaytsev
 *
 * Is distributed under the GPL 2 license.
 *
 */

#ifndef __SCHEDULER_H__
#define __SCHEDULER_H__

#include <thread.h> /* for struct thread_t */

int scheduler_add_to_queue(struct thread_t * thread);
struct thread_t * scheduler_get_next_thread();
void scheduler_start();
void scheduler_stop();
void scheduler_init();
void scheduler_wake();

#endif /* __SCHEDULER_H__ */
