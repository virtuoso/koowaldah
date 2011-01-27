/*
 * konfig/kout.c
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


