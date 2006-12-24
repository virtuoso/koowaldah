/*
 * konfig/konfigure.c
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
#include "koption.inc"

/* 
 * Input/output files.
 * Allow for these to be defined from the outside.
 */
#ifndef KONFIG_PATH
#define KONFIG_PATH "./KONFIG"
#endif

#ifndef HEADER_PATH
#define HEADER_PATH "./include/koptions.h"
#endif

#ifndef MAKE_PATH
#define MAKE_PATH "./konfig.mk"
#endif

extern unsigned long __start_KPTR;
extern unsigned long __stop_KPTR;

unsigned int total_kopts = 0;
koption_t *KOPT;
static int nointr = 0;

void init()
{
	void *p;
	koption_t *kopt;
	ksection_t *ksec;
	int i, n;

	p = &__start_KPTR;
	n = 0;

	KOPT = NULL;

	/* scan through and allocate KOPT array */
	while (p < (void *)&__stop_KPTR) {
		kopt = DEREF(p, koption_t);
		ksec = DEREF(p + 4, ksection_t);
		n = ksec->items;

		for (i = 0; i < n; i++) {
			KOPT = realloc(KOPT, 
					(total_kopts + 1) * sizeof(koption_t));
			if (!KOPT) {
				fprintf(stderr, "ran out of memory\n");
				exit(EXIT_FAILURE);
			}

			memcpy(KOPT + total_kopts, kopt, sizeof(koption_t));
			KOPT[total_kopts].section = ksec;
			kopt++;
			total_kopts++;
		}
		p += 8;
	}
}

int kopt_setval(const char *name, char *val)
{
	int i;

	/* if name starts with "OPT_", disregard it */
	if (!strncmp("OPT_", name, 4))
		name += 4;

	for (i = 0; i < total_kopts; i++)
		if (!strcmp(KOPT[i].name, name)) {
			switch (KOPT[i].type) {
				case KOPT_BOOL:
					KOPT[i].def.b.v = val[0];
					break;
				case KOPT_TRISTATE:
					KOPT[i].def.t.v = val[0];
					break;
				case KOPT_INT:
					KOPT[i].def.i.v = atoi(val);
					break;
				case KOPT_STRING:
					KOPT[i].def.s.v = strdup(val);
					break;
			}
			KOPT[i].user_set++;
			return 0;
		}
	return -1;
}

/*
 * Check if a bool or tristate option has it's value set
 * @name -- the option
 * returns: 0 == not set ('N')
 *          1 == set to 'Y'
 *          2 == set to 'M' (for tristates)
 *         -1 == no option with this name exists or nonbool type
 */
int kopt_isset(const char *name)
{
	int i;
	char v;

	/* if name starts with "OPT_", disregard it */
	if (!strncmp("OPT_", name, 4))
		name += 4;

	for (i = 0; i < total_kopts; i++)
		if (!strcmp(KOPT[i].name, name)) {
			switch (KOPT[i].type) {
				case KOPT_BOOL:
					v = KOPT[i].def.b.v;
					break;
				case KOPT_TRISTATE:
					v = KOPT[i].def.t.v;
					break;
				default:
					return -1;
			}

			if (v == 'M')
				return 2;

			return v == 'Y' ? 1 : 0;
		}
	return -1;
}

void read_konfig()
{
	FILE *config;
	char line[80], *ps, *pe;
	char opt[OPTNAME_MAX], val[OPTNAME_MAX];

	config = fopen(KONFIG_PATH, "rw");
	if (config) {

		while (!feof(config)) {
			memset(opt, 0, OPTNAME_MAX);
			memset(val, 0, OPTNAME_MAX);
			fscanf(config, "%79[^\n]\n", line);
			ps = line;
			while (*ps == ' ' || *ps == '\t') ps++;
			if (*ps == '#') continue;

			pe = strchr(ps, '=');
			if (!pe) continue;

			strncpy(opt, ps, pe-ps);
			strcpy(val, pe+1);
			if (kopt_setval(opt, val))
				printf("No such option: %s\n", opt);
		}
		fclose(config);
	}
}

int main()
{
	nointr = 1;
	init();
	read_konfig();
	if (!nointr)
		ask_user();

	process_konfig();
	writeback(KONFIG_PATH, HEADER_PATH, MAKE_PATH);
	free(KOPT);

	return 0;
}

