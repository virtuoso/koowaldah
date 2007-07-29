/*
 * include/dummy/pages.h
 *
 * Copyright (C) 2007 Alexander Shishkin
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Koowaldah developers nor the names of their
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
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

#ifndef __ARCH_PAGES_H__
#define __ARCH_PAGES_H__

/* page size, bits, mask, unmask */
#define PAGE_SHIFT 12
#define PAGE_SIZE (1UL << PAGE_SHIFT)
#define PAGE_MASK (PAGE_SIZE - 1)
#define NOPAGE_MASK ~(PAGE_MASK)
#define NOPAGE_ADDR (~0UL)

#define PTF_PRESENT  (1 << 0)  /* page/pte is present    */
#define PTF_RW       (1 << 1)  /* write access available */
#define PTF_USER     (1 << 2)  /* accessible by user     */
#define PTF_PWT      (1 << 3)  /* prevent write-through  */
#define PTF_PCD      (1 << 4)  /* cache disable          */
#define PTF_ACCESSED (1 << 5)  /* page has been accessed */
#define PTF_DIRTY    (1 << 6)  /* page is dirty          */
#define PTF_PGSIZE   (1 << 7)  /* page size = 4M         */
#define PTF_GLOBAL   (1 << 8)  /* global (for TLB)       */

static __inline u32 __virt2physpg(u32 pgaddr)
{
	return (pgaddr & NOPAGE_MASK);
}

static __inline u32 __virt2phys(u32 addr)
{
	return addr;
}

#endif

