/*
 * konfig/koption.h
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

/*
 * This is KOPT, the KOS kernel configuration system
 */
#define OPTNAME_MAX 32

/*
 * Available option types
 */
enum {
	KOPT_BOOL,
	KOPT_TRISTATE,
	KOPT_INT,
	KOPT_STRING
};

typedef struct kopt_bool {
	unsigned char v;
} kopt_bool_t;

typedef struct kopt_tristate {
	unsigned char v;
} kopt_tristate_t;

typedef struct kopt_int {
	unsigned int v;
} kopt_int_t;

typedef struct kopt_string {
	const char *v;
} kopt_string_t;

/* config file section */
typedef struct ksection {
	const char *desc;
	const int items;
} ksection_t;

typedef struct koption {
	const char name[OPTNAME_MAX];
	const char *desc;
	const char *help;
	const int type;
	union {
		kopt_bool_t     b;
		kopt_int_t      i;
		kopt_tristate_t t;
		kopt_string_t   s;
	} def;
	int user_set;
	ksection_t *section;
} koption_t;

#define DEREF(x, type) ({ char **__p = (char **)(x); (type *)*__p; })

#define __PTR __attribute__((section("KPTR")))
#define KOPTIONS_START(__name, __desc, __number) \
	const koption_t __name ## _options[__number]; \
	const ksection_t __name ## _section = { \
		.desc = __desc, \
		.items = __number \
	}; \
	const void __PTR *__name ## _options_ptr = &__name ## _options; \
	const void __PTR *__name ## _desc_ptr = &__name ## _section; \
	const koption_t __name ## _options[__number] = {
#define KOPTIONS_END };
#define KOPTION(__name, __desc, __help, __type, __default) { \
	.name = __name, \
	.desc = __desc, \
	.help = __help, \
	.type = __type, \
	.def = DEF_ ## __type(__default), \
	.user_set = 0, \
	.section = NULL \
}

#define DEF_KOPT_BOOL(x) { .b = { .v = (x) } }
#define DEF_KOPT_TRISTATE(x) { .t = { .v = (x) } }
#define DEF_KOPT_INT(x) { .i = { .v = (x) } }
#define DEF_KOPT_STRING(x) { .s = { .v = (x) } }

/* kout.c */
void writeback(const char *konfig,
	       const char *header,
	       const char *make);
/* kuser.c */
void ask_user();

