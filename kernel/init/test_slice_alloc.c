/*
 * kernel/init/test_slice_alloc.c
 *
 * Copyright (C) 2006 Alexey Zaytsev
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

