
/*
 * kernel/textio.c
 *
 * Copyright (C) 2005 Alexander Shishckin
 *
 * Is distributed under the GPL 2 license.
 *
 */

#include <kuca.h>
#include <textio.h>
#include <lib.h>
#include <console.h>

static char *digits="0123456789ABCDEF";
#define PRINTF_BUFSIZE 1024

static size_t sprint_int( char *buf, long i, char base ) {
    int pos = 1;
    int k = 1, l = base;

    if(i < 0) {
        buf[pos++ - 1] = '-';
        i = 0 - i;
    }

    while ( i / l != 0 ) {
        l *= base;
        pos++;
    } 

    do {
        buf[pos - k++] = digits[i % base];
        i /= base;
    } while(0 != i);

    return pos;
}

static void print(const char *message){
    while(*message)
        console_put_char(*(message++));
}

int vsnprintf( char *buf, size_t size, const char *format, va_list args )
{
    int flen = string_len( (char *)format );
    char *cur = (char *)format;
    char *old = (char *)format;
    char *pos = buf;
    /* args */
    long a_x;
    /*char a_c;*/
    char sbuf[12]; /* <-- this is evil also */
    char *a_s;
    
    /* parse <format> */
    do {
        cur = find_char( cur, '%' );
        if( NULL != cur )
            pos += memory_copy( pos, old, cur - old );

        if( NULL == cur || '\0' == *cur++ )
            break;

        switch( *cur ) {
            case 'd':
                memory_set( sbuf, 0, 12 );
                a_x = va_arg( args, int );
                a_x = sprint_int( sbuf, a_x, 10 );
                pos += memory_copy( pos, sbuf, a_x );
                break;
                
            case 'l':
                memory_set( sbuf, 0, 12 );
                a_x = va_arg( args, long );
                a_x = sprint_int( sbuf, a_x, 10 );
                pos += memory_copy( pos, sbuf, a_x );
                break;
                
            case 'x':
                memory_set( sbuf, 0, 12 );
                a_x = va_arg( args, long );
                a_x = sprint_int( sbuf, a_x, 16 );
                pos += memory_copy( pos, sbuf, a_x );
                break;
                
#if 0
            case 'c':
                a_c = va_arg( args, char );
                *pos++ = a_c;
                break;
#endif

            case 's':
                a_s = va_arg( args, char * );
                pos += memory_copy( pos, a_s, string_len(a_s) - 1 );
                break;

            default:
                print( "Hell is that? '" );
                console_put_char( *cur );
                print( "'\n" );
                /*print( "You surely aren't expecting a full-blown kprintf here?\n" );*/
        }
        old = cur + 1;
    } while( cur < format + flen );

    pos += memory_copy( pos, old, format + flen - old );
    
    return pos - buf;
}

int snprintf( char *buf, size_t size, const char *fmt, ... )
{
    va_list args;
    int i;

    va_start( args, fmt );
    i = vsnprintf( buf,size,fmt,args );
    va_end( args );
    return i;
}

int kprintf( const char *fmt, ... )
{
    va_list args;
    int i;
    char buf[PRINTF_BUFSIZE];

    va_start( args, fmt );
    i = vsnprintf( buf, PRINTF_BUFSIZE, fmt, args );
    va_end( args );
    print( buf );

    return i;
}

