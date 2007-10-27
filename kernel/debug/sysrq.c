/*
 * kernel/debug/sysrq.c
 *
 * Copyright (C) 2007 Alexander Shishkin
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Koowaldah developers nor the names of their
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
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

