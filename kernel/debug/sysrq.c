/*
 * kernel/debug/sysrq.c
 *
 * Copyright (C) 2007 Alexander Shishkin
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <koowaldah.h>
#include <scheduler.h> /* for list_threads() */
#include <mem_area.h>  /* for display_map() */

/* sysrq key pressed */
static int sysrq_acq = 0;

void sysrq_usage()
{
	kprintf(
		"SYSRQ usage:\n"
		" * SYSRQ-p: display current kernel thread.\n"
		" * SYSRQ-P: above + call trace.\n"
		" * SYSRQ-m: above + memory mapping.\n"
		" * SYSRQ-t: display all kernel threads.\n"
		" * SYSRQ-T: above + call traces.\n"
		" * SYSRQ-M: above + memory mappings.\n"
		" * SYSRQ-*: display this message.\n"
#ifdef OPT_INSTRUMENT_PROFILER
		" * SYSRQ-{y,Y}: flush instrument profiler's events.\n"
#endif /* OPT_INSTRUMENT_PROFILER */
	);
}

/*
 * Check for sysrq action trigger and perform said action.
 * Called with an ascii character code or 0xff to denote
 * 'SYSRQ'.
 * @n -- character code
 * @returns 1 if SYSRQ action was performed or 0 otherwise.
 */
int sysrq_hit(char n)
{
	u32 flags = 0;

	if (n == 0xff)
		sysrq_acq++;
	else if (sysrq_acq) {
		switch (n) {
			case 'f':
				dump_inodes();
				break;

			case 'm':
				flags = LIST_MAPPING;
			case 'P':
				flags |= LIST_BACKTRACE;
			case 'p':
				list_threads(flags | LIST_CURRENT);
				break;

			case 'M':
				flags = LIST_MAPPING;
			case 'T':
				flags |= LIST_BACKTRACE;
			case 't':
				list_threads(flags);
				break;

#ifdef OPT_INSTRUMENT_PROFILER
			case 'y':
			case 'Y':
				flush_events();
				break;
#endif /* OPT_INSTRUMENT_PROFILER */

			default:
				sysrq_usage();
		}

		sysrq_acq = 0;
	} else
		return 0;

	return 1;
}

