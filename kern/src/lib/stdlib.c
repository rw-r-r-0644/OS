#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <mm/heap.h>

__attribute__((noreturn))
void abort()
{
	printf("KERNEL PANIC: abort();\n");
	asm volatile("hlt");
	while (1) { }
	__builtin_unreachable();
}

int atoi (const char * s)
{
	if (s == NULL)
		return 0;

	int res = 0;
	int sign = 1;

	if (*s == '-') // < 0
	{
		sign = -1;
		s++;
	}
	while (*s != '\0' && *s >= '0' && *s <= '9')
		res = res * 10 + (*(s++) - '0');
  
	return sign * res;
}

void * malloc(size_t size)
{
	return heapalloc(size);
}

void free(void *ptr)
{
	return heapfree(ptr);
}

void * calloc(size_t num, size_t size)
{
	size_t nbytes = num * size;
	void * ptr;

	ptr = malloc(nbytes);
	if (ptr)
		memset(ptr, 0, nbytes);

	return ptr;
}
