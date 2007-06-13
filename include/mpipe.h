#ifndef __MPIPE_H__
#define __MPIPE_H__

int mpipe_open(char *name);

/* XXX: move to kernel/fs/fs.h */
void __init mpipe_init();

#endif
