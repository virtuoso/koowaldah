
/*
 * System call numbers and functions
 */

#define SYS_debug 0
#define SYS_open 1
#define SYS_read 2
#define SYS_tsleep 3

int sys_debug(char *str);
int sys_open(char *name, unsigned int mode);
int sys_read(int fd, char *buf, unsigned int len);
int sys_tsleep(unsigned int t);

