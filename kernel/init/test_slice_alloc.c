/*
 * kernel/init/test_slice_alloc.c
 *
 * Copyright (C) 2006 Alexey Zaytsev
 * Copyright (C) 2006 Alexander Shishkin
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
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 * 
 */
#include <koowaldah.h>
#include <textio.h>
#include <klist0.h>
#include <slice.h>


u32 * slices[1024];

void test_slice_alloc()
{
#ifdef OPT_TEST_SLICE
	struct slice_pool *slice;
	int i;

	kprintf("Testing allocator with 4-byte slices: \n");

	slice = slice_pool_create(0, 4);
	kprintf("Created a fresh slice allocator\n");

	slice_pool_info(slice);

	kprintf("Allocating 1024 4-byte slices.\n");
	
	for (i = 0; i < 1024; i++) {
		/* kprintf("Allocating object %d\n", i); */
		slices[i] = slice_alloc(slice);
		/* kprintf("Allocated object %d\n", i); */
		/* lice_info(slice); */
	}

	kprintf("Done with allocating 1024 4-byte objects.\n");

	slice_pool_info(slice);

	
	
	/*
	for (i = 0; i < 1024; i++) {
		kprintf("object %d = 0x%x\n", i, objects[i]);
	}
	*/
	
	kprintf("Freeing even slices\n");
	for (i = 0; i < 1024; i = i + 2) {
		/* kprintf("Freeind object %d\n", i); */
		slice_free(slices[i], slice);
		/* kprintf("Done freeing object %d\n", i);*/
		/* slice_pool_info(slice); */
	}
	
	kprintf("Done freeing even pages.\n");
	slice_pool_info(slice);

	kprintf("Allocating even slices back.\n");

	for (i = 0; i < 1024; i = i + 2) {
		slices[i] = slice_alloc(slice);
	}

	kprintf("Done allocating even slices back.\n");

	slice_pool_info(slice);

	kprintf("Freeing odd slices.\n");

	for (i = 1; i < 1024; i = i + 2) {
		slice_free(slices[i], slice);
	}

	kprintf("Done freeing odd slices.\n");

	slice_pool_info(slice);

	kprintf("Freeing even slices.\n");

	for (i = 0; i < 1024; i = i + 2) {
		slice_free(slices[i], slice);
	}

	kprintf("Done freeing even slices.\n");

	slice_pool_info(slice);

	kprintf("Recycling slice pool.\n");

	slice_pool_recycle(slice);

	kprintf("Done recycling slice pool.\n");

	slice_pool_info(slice);
		

	kprintf("Done with testing slice allocator on 4-byte slices.\n");
	
	kprintf("Testing allocator with 40-byte slices: \n");

	slice = slice_pool_create(0, 40);
	kprintf("Created a fresh slice allocator\n");

	slice_pool_info(slice);

	kprintf("Allocating 1024 40-byte slices.\n");
	
	for (i = 0; i < 1024; i++) {
		/* kprintf("Allocating object %d\n", i); */
		slices[i] = slice_alloc(slice);
		/* kprintf("Allocated object %d\n", i); */
		/* lice_info(slice); */
	}

	kprintf("Done with allocating 1024 40-byte objects.\n");

	slice_pool_info(slice);

	
	
	/*
	for (i = 0; i < 1024; i++) {
		kprintf("object %d = 0x%x\n", i, objects[i]);
	}
	*/
	
	kprintf("Freeing even slices\n");
	for (i = 0; i < 1024; i = i + 2) {
		/* kprintf("Freeind object %d\n", i); */
		slice_free(slices[i], slice);
		/* kprintf("Done freeing object %d\n", i);*/
		/* slice_pool_info(slice); */
	}
	
	kprintf("Done freeing even pages.\n");
	slice_pool_info(slice);

	kprintf("Allocating even slices back.\n");

	for (i = 0; i < 1024; i = i + 2) {
		slices[i] = slice_alloc(slice);
	}

	kprintf("Done allocating even slices back.\n");

	slice_pool_info(slice);

	kprintf("Freeing odd slices.\n");

	for (i = 1; i < 1024; i = i + 2) {
		slice_free(slices[i], slice);
	}

	kprintf("Done freeing odd slices.\n");

	slice_pool_info(slice);

	kprintf("Freeing even slices.\n");

	for (i = 0; i < 1024; i = i + 2) {
		slice_free(slices[i], slice);
	}

	kprintf("Done freeing even slices.\n");

	slice_pool_info(slice);

	kprintf("Recycling slice pool.\n");

	slice_pool_recycle(slice);

	kprintf("Done recycling slice pool.\n");

	slice_pool_info(slice);

	kprintf("Done with testing slice allocator on 40-byte slices.\n");
	
	kprintf("Testing allocator with 200-byte slices: \n");

	slice = slice_pool_create(0, 200);
	kprintf("Created a fresh slice allocator\n");

	slice_pool_info(slice);

	kprintf("Allocating 1024 200-byte slices.\n");
	
	for (i = 0; i < 1024; i++) {
		/* kprintf("Allocating object %d\n", i); */
		slices[i] = slice_alloc(slice);
		/* kprintf("Allocated object %d\n", i); */
		/* lice_info(slice); */
	}

	kprintf("Done with allocating 1024 200-byte objects.\n");

	slice_pool_info(slice);

	
	
	/*
	for (i = 0; i < 1024; i++) {
		kprintf("object %d = 0x%x\n", i, objects[i]);
	}
	*/
	
	kprintf("Freeing even slices\n");
	for (i = 0; i < 1024; i = i + 2) {
		/* kprintf("Freeind object %d\n", i); */
		slice_free(slices[i], slice);
		/* kprintf("Done freeing object %d\n", i);*/
		/* slice_pool_info(slice); */
	}
	
	kprintf("Done freeing even pages.\n");
	slice_pool_info(slice);

	kprintf("Allocating even slices back.\n");

	for (i = 0; i < 1024; i = i + 2) {
		slices[i] = slice_alloc(slice);
	}

	kprintf("Done allocating even slices back.\n");

	slice_pool_info(slice);

	kprintf("Freeing odd slices.\n");

	for (i = 1; i < 1024; i = i + 2) {
		slice_free(slices[i], slice);
	}

	kprintf("Done freeing odd slices.\n");

	slice_pool_info(slice);

	kprintf("Freeing even slices.\n");

	for (i = 0; i < 1024; i = i + 2) {
		slice_free(slices[i], slice);
	}

	kprintf("Done freeing even slices.\n");

	slice_pool_info(slice);

	kprintf("Recycling slice pool.\n");

	slice_pool_recycle(slice);

	kprintf("Done recycling slice pool.\n");

	slice_pool_info(slice);

	kprintf("Done with testing slice allocator on 200-byte slices.\n");

#endif /* OPT_TEST_SLICE */
}

