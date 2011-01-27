/*
 * konfig/kuser.c
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
 * KONFIG user interaction module
 */

extern unsigned int total_kopts;
extern koption_t *KOPT;

/* interrogation buffer */
static char answer[80];

void put_bool_answer(koption_t *kopt)
{
entry:
	switch (kopt->def.b.v) {
		case 'y':
		case 'Y':
			printf("[Y,n,help]: ");
			break;

		case 'n':
		case 'N':
			printf("[y,N,help]: ");
			break;

		default:
			printf("invalid bool value %c\n", kopt->def.b.v);
	}

	scanf("%[^\n]\n", answer);
	switch (answer[0]) {
		case 'y':
		case 'Y':
			kopt->def.b.v = 'Y';
			break;

		case 'n':
		case 'N':
			kopt->def.b.v = 'N';
			break;

		case 'h':
		case 'H':
			printf("\n\n%s (%s)\n-------------\n%s\n",
					kopt->desc,
					kopt->name,
					kopt->help);

		case '\0':
			printf("%c\n", kopt->def.b.v);
			getchar();
			break;

		default:
			goto entry;
	}
}

void put_tristate_answer(koption_t *kopt)
{
entry:
	switch (kopt->def.t.v) {
		case 'y':
		case 'Y':
			printf("[Y,n,m,help]: ");
			break;

		case 'n':
		case 'N':
			printf("[y,N,m,help]: ");
			break;

		case 'm':
		case 'M':
			printf("[y,n,M,help]: ");
			break;

		default:
			printf("invalid bool value %c\n", kopt->def.t.v);
	}

	scanf("%[^\n]\n", answer);
	switch (answer[0]) {
		case 'y':
		case 'Y':
			kopt->def.t.v = 'Y';
			break;

		case 'n':
		case 'N':
			kopt->def.t.v = 'N';
			break;

		case 'm':
		case 'M':
			kopt->def.t.v = 'M';
			break;

		case 'h':
		case 'H':
			printf("\n\n%s (%s)\n-------------\n%s\n",
					kopt->desc,
					kopt->name,
					kopt->help);

		case '\0':
			printf("%c\n", kopt->def.t.v);
			getchar();
			break;

		default:
			goto entry;
	}
}

void put_int_answer(koption_t *kopt)
{
	int i;

	printf("[%d]: ", kopt->def.i.v);
	scanf("%d", &i);
	kopt->def.i.v = i;
}

void put_string_answer(koption_t *kopt)
{
	printf("[%s]: ", kopt->def.s.v);
}

void ask_option(koption_t *kopt)
{
	char *s;
	printf("%s: %s ", kopt->name, kopt->desc);

	switch (kopt->type) {
		case KOPT_BOOL:
			put_bool_answer(kopt);
			break;

		case KOPT_TRISTATE:
			put_tristate_answer(kopt);
			break;

		case KOPT_INT:
			put_int_answer(kopt);
			break;

		case KOPT_STRING:
			put_string_answer(kopt);
			s = malloc(80);
			scanf("%79[^\n]\n", s);
			if (s[0] != '\0')
				kopt->def.s.v = s;
			else
				getchar();
			break;

		default:
			fprintf(stderr, "invalid option type %d\n", kopt->type);
			exit(EXIT_FAILURE);
	}
	kopt->user_set++;
}

void ask_user()
{
	int i;
	ksection_t *ksec = NULL;

	for (i = 0; i < total_kopts; i++) {
		if (KOPT[i].section != ksec) {
			if (ksec) printf("\n\n");
			ksec = KOPT[i].section;
			printf("/*\n * %s\n * (%d options)\n */\n\n",
					ksec->desc, ksec->items);
		}
		
		if (!KOPT[i].user_set)
			ask_option(&KOPT[i]);
	}
}

