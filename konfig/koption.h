/*
 * konfig/koption.h
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

/*
 * This is KOPT, the KOS kernel configuration system
 */
#define OPTNAME_MAX 32
#define VALUE_MAX 256
#define CONFLICT_MAX 32

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
	const char *conflicts[CONFLICT_MAX];
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

#define ARRSIZE(a) ({ \
		unsigned int __i = 0; \
		for (__i = 0; a[__i]; __i++); \
		__i; \
	})

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
#define KOPTION_FULL(__name, __desc, __help, __type, __conflicts, __default) { \
	.name = __name, \
	.desc = __desc, \
	.help = __help, \
	.type = __type, \
	.conflicts = __conflicts, \
	.def = DEF_ ## __type(__default), \
	.user_set = 0, \
	.section = NULL \
}
#define KOPTION(__name, __desc, __help, __type, __default) \
	KOPTION_FULL(__name, __desc, __help, __type, {NULL}, __default)

#define CONFLICTS(args ...) { args, NULL }

#define DEF_KOPT_BOOL(x) { .b = { .v = (x) } }
#define DEF_KOPT_TRISTATE(x) { .t = { .v = (x) } }
#define DEF_KOPT_INT(x) { .i = { .v = (x) } }
#define DEF_KOPT_STRING(x) { .s = { .v = (x) } }

/* konfigure.c, generic helpers */
int kopt_isset(const char *name);
int kopt_setval(const char *name, char *val);

/* kout.c */
void writeback(const char *konfig,
	       const char *header,
	       const char *make);
/* kuser.c */
void ask_user();

/* ktree.c */
void process_konfig();

