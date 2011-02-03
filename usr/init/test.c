#include <stdlib.h>
#include <stdio.h>
#include <tsleep.h>

int main()
{
	puts("Hello, koowaldah!\n");
	for (;;) {
		putchar('C');
		tsleep(90);
	}
	return 0;
}
