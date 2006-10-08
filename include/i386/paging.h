
/*
 * include/paging.h
 *
 * Copyright (C) 2005 Alexey Zaytsev
 *
 * Is distributed under the GPL 2 license.
 *
 */

int paging_init();

void * page_get();
void page_put(u8 * page);
