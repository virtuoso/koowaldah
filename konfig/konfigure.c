/*
 * config/configure.c
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

#define KONFIG_PATH "./KONFIG"
#define OUTPUT_PATH "./include/koptions.h"

extern unsigned long __start_KPTR;
extern unsigned long __stop_KPTR;

static unsigned int total_kopts = 0;
static koption_t *KOPT;
static FILE *out = NULL;
static FILE *out_header = NULL;
static int nointr = 0;

void print_option(koption_t *kopt)
{
	fprintf(out, "/* %s */\n", kopt->desc);
	fprintf(out, "OPT_%s=", kopt->name);
	switch (kopt->type) {
		case KOPT_BOOL:
			fputc(kopt->def.b.v, out);
			break;

		case KOPT_TRISTATE:
			fputc(kopt->def.t.v, out);
			break;

		case KOPT_INT:
			fprintf(out, "%d", kopt->def.i.v);
			break;

		case KOPT_STRING:
			fprintf(out, "%s", kopt->def.s.v);
			break;

		default:
			fprintf(stderr, "invalid option type %d\n", kopt->type);
			exit(EXIT_FAILURE);
	}

	fputc('\n', out);
}

void define_option(koption_t *kopt)
{
	fprintf(out_header, "/* %s */\n", kopt->desc);
	switch (kopt->type) {
		case KOPT_BOOL:
			if (kopt->def.b.v == 'Y')
				fprintf(out_header, "#define OPT_%s\n",
						kopt->name);
			else
				fprintf(out_header, "#undef OPT_%s\n",
						kopt->name);
			break;

		case KOPT_TRISTATE:
			if (kopt->def.t.v == 'Y' ||
			    kopt->def.t.v == 'M')
				fprintf(out_header, "#define OPT_%s\n",
						kopt->name);
			else
				fprintf(out_header, "#undef OPT_%s\n",
						kopt->name);
			break;

		case KOPT_INT:
			fprintf(out_header, "#define OPT_%s %d\n",
					kopt->name,
					kopt->def.i.v);
			break;

		case KOPT_STRING:
			fprintf(out_header, "#define OPT_%s %s\n",
					kopt->name,
					kopt->def.s.v);
			break;

		default:
			fprintf(stderr, "invalid option type %d\n", kopt->type);
			exit(EXIT_FAILURE);
	}
}

void print_all()
{
	int i;
	ksection_t *ksec = NULL;

	for (i = 0; i < total_kopts; i++) {
		if (KOPT[i].section != ksec) {
			if (ksec) {
				fprintf(out, "\n\n");
				fprintf(out_header, "\n\n");
			}
			ksec = KOPT[i].section;
			fprintf(out, "/*\n * %s\n * (%d options)\n */\n\n",
					ksec->desc, ksec->items);
			fprintf(out_header,
					"/*\n * %s\n * (%d options)\n */\n\n",
					ksec->desc, ksec->items);
		}
		print_option(&KOPT[i]);
		define_option(&KOPT[i]);
	}
}

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

void read_konfig()
{
	int fd = open(KONFIG_PATH, O_RDONLY);
	if (!fd)
		return;
	close(fd);

	out = fopen(KONFIG_PATH, "w");
	if (!out)
		exit(EXIT_FAILURE);
	out_header = fopen(OUTPUT_PATH, "w");
	if (!out_header)
		exit(EXIT_FAILURE);
}

/* interrogation buffer */
char answer[80];

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

int main()
{
	nointr = 1;
	init();
	read_konfig();
	if (!nointr)
		ask_user();

	print_all();
	free(KOPT);
	fclose(out);
	fclose(out_header);

	return 0;
}

