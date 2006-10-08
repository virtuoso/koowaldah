
/*
 * include/textio.h
 *
 * Copyright (C) 2005 Alexey Zaytsev
 *
 * Is distributed under the GPL 2 license.
 *
 */

#ifndef __TEXTIO_H__
#define __TEXTIO_H__

#include <stdarg.h>

int vsnprintf( char *buf, size_t size, const char *format, va_list args );
int snprintf(char *buf, size_t size, const char *fmt, ...);
int printf( const char *fmt, ... );

#endif /* __TEXTIO_H__ */

