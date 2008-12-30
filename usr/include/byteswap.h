/*
 * byteswap.h
 */

#ifndef _BYTESWAP_H
#define _BYTESWAP_H

#include <klibc/compiler.h>
//#include <asm/byteorder.h>
#include <khui/i386/endian.h>

#define bswap_16(x) __swab16(x)
#define bswap_32(x) __swab32(x)
#define bswap_64(x) __swab64(x)

#endif				/* _BYTESWAP_H */
