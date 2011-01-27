/*
 * konfig/ktree.c
 *
 * Copyright (C) 2006 Alexander Shishkin
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


