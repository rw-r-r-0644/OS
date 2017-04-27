#include <string.h>

void * memcpy (void *s1, const void *s2, size_t n)
{
	while (n--)
		*((u8 *)s1++) = *((u8 *)s2++);
	return s1;
}