#include <string.h>

void * memchr (const void *s, int c, size_t n)
{
	while (n--)
		if (*((u8 *)s) == (u8)c)
			return (void *)s;
		else
			++s;
	return NULL;
}