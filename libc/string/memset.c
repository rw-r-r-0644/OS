#include <string.h>

void * memset (void *s, int c, size_t n)
{
	while (n--)
		*((u8 *)s++) = (u8)c;
	return s;
}