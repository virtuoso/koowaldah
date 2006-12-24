/*
 * konfig/ktree.c
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
 * 3. Neither the name of the Koowaldah developers nor the names of theyr
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "koption.h"

/*
 * KONFIG options processing module.
 *
 * I expect the logic to remain severely dumb forever, but a bit smarter
 * than it is now.
 * 1. Conflicting options.
 * The first conflicting option found in the list always prevails: those
 * listed in option's conflict list are automatically unset and a warning
 * is spit out.
 * NOTE: it only makes sense for booleans and tristates to conflict with
 *       each other.
 */

extern unsigned int total_kopts;
extern koption_t *KOPT;

void process_konfig()
{
	int i, k, c;

	for (i = 0; i < total_kopts; i++) {
		c = ARRSIZE(KOPT[i].conflicts);
		if (kopt_isset(KOPT[i].name) > 0 && c) {
			/* FIXME: this is here for debugging purposes only */
			fprintf(stderr, "### option %s has conflicts: ",
					KOPT[i].name);
			
			for (k = 0; k < c; k++) {
				fprintf(stderr, "%s ", KOPT[i].conflicts[k]);
				if (kopt_isset(KOPT[i].conflicts[k]) > 0) {
					fprintf(stderr, "[SET!] ");
					kopt_setval(KOPT[i].conflicts[k], "N");
				}
			}
			fprintf(stderr, "\n");
		}
	}
}


