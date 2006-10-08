
/*
 * kernel/klib.c
 *
 * Copyright (C) 2005 Alexey Zaytsev
 *
 * Is distributed under the GPL 2 license.
 *
 */

#include <kuca.h>
#include <lib.h> 


size_t memory_set(void *s, char c, size_t n){
	
	size_t t = n;
	while(t-- > 0)
		((char *)s)[t] = (char)c;
	return n;
}

size_t memory_copy( void *dest, void *src, size_t n )
{
	size_t i;

	if(dest == src )
		return n;

	if(dest < src)
		for( i = 0; i < n; i++ )
			((char *)dest)[i] = ((char *)src)[i];
	else
		for( i = n; i > 0; i--)
			((char *)dest)[i-1] = ((char *)src)[i-1];
				
	return n;
}

size_t string_len( char *str )
{
    char *p = str;

    while( '\0' != *p++ );
    return p - str;
}

char *find_char( char *str, char c )
{
    char *p = str;

    while( '\0' != *p && c != *p ) p++;
    if( '\0' == *p ) return NULL;

    return p;
}

