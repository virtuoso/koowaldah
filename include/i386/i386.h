/*
 * Compile-time sanity check
 * Turns out to be pretty useful.
 */

#ifndef CONFIG_X86
#error Never, ever include this file on non-x86 architectures.
#endif
#ifndef __i386__
#error Non-x86 or x86? Make up your mind
#endif
