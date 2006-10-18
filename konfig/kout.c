/*
 * konfig/kout.c
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
 * KONFIG output drivers for KONFIG, c header and gnu make formats.
 */

extern unsigned int total_kopts;
extern koption_t *KOPT;

/* write an option (back) to KONFIG file */
void kopt2konfig(FILE *out, koption_t *kopt)
{
	fprintf(out, "# %s\n", kopt->desc);
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

/* write an option to config header */
void kopt2header(FILE *out, koption_t *kopt)
{
	fprintf(out, "/* %s */\n", kopt->desc);
	switch (kopt->type) {
		case KOPT_BOOL:
			if (kopt->def.b.v == 'Y')
				fprintf(out, "#define OPT_%s\n",
						kopt->name);
			else
				fprintf(out, "#undef OPT_%s\n",
						kopt->name);
			break;

		case KOPT_TRISTATE:
			if (kopt->def.t.v == 'Y' ||
			    kopt->def.t.v == 'M')
				fprintf(out, "#define OPT_%s\n",
						kopt->name);
			else
				fprintf(out, "#undef OPT_%s\n",
						kopt->name);
			break;

		case KOPT_INT:
			fprintf(out, "#define OPT_%s %d\n",
					kopt->name,
					kopt->def.i.v);
			break;

		case KOPT_STRING:
			fprintf(out, "#define OPT_%s \"%s\"\n",
					kopt->name,
					kopt->def.s.v);
			break;

		default:
			fprintf(stderr, "invalid option type %d\n", kopt->type);
			exit(EXIT_FAILURE);
	}
}

void kopt2make(FILE *out, koption_t *kopt)
{
	fprintf(out, "# %s\n", kopt->desc);
	fprintf(out, "MK_%s := ", kopt->name);
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

/* write a section desctiption */
void ksec2konfig(FILE *out, ksection_t *ksec)
{
	fprintf(out, "# %s\n# (%d options)\n#\n\n",
			ksec->desc, ksec->items);
}

void ksec2header(FILE *out, ksection_t *ksec)
{
	fprintf(out, "/*\n * %s\n * (%d options)\n */\n\n",
			ksec->desc, ksec->items);
}

void ksec2make(FILE *out, ksection_t *ksec)
{
	fprintf(out, "# %s\n# (%d options)\n#\n\n",
			ksec->desc, ksec->items);
}

/* put some whitespace */
void ws2konfig(FILE *out)
{
	fprintf(out, "\n\n");
}

void ws2header(FILE *out)
{
	fprintf(out, "\n\n");
}

void ws2make(FILE *out)
{
	fprintf(out, "\n\n");
}

void writeback(const char *konfig,
	       const char *header,
	       const char *make)
{
	FILE *konfig_file;
	FILE *header_file;
	FILE *make_file;
	int i;
	ksection_t *ksec = NULL;

	konfig_file = fopen(konfig, "w");
	if (!konfig_file)
		exit(EXIT_FAILURE);
	header_file = fopen(header, "w");
	if (!header_file)
		exit(EXIT_FAILURE);
	make_file = fopen(make, "w");
	if (!header_file)
		exit(EXIT_FAILURE);

	for (i = 0; i < total_kopts; i++) {
		if (KOPT[i].section != ksec) {
			if (ksec) {
				ws2konfig(konfig_file);
				ws2header(header_file);
				ws2make(make_file);
			}
			ksec = KOPT[i].section;
			ksec2konfig(konfig_file, ksec);
			ksec2header(header_file, ksec);
			ksec2make(make_file, ksec);
		}
		kopt2konfig(konfig_file, &KOPT[i]);
		kopt2header(header_file, &KOPT[i]);
		kopt2make(make_file, &KOPT[i]);
	}

	fclose(konfig_file);
	fclose(header_file);
	fclose(make_file);
}


