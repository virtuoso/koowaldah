
/*
 * include/lib.h
 *
 * Copyright (C) 2005 Alexey Zaytsev
 *
 * Is distributed under the GPL 2 license.
 *
 */

#ifndef __LIB_H__
#define __LIB_H__

size_t memory_set(void *s, char c, size_t n);
size_t memory_copy( void *dest, void *src, size_t n );


size_t string_len( char *str );
char * find_char( char *str, char c );

#endif /* __LIB_H__ */

