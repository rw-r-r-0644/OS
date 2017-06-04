#include <string.h>

void * memset (void *s, int c, size_t n)
{
	u8* m = (u8*)s;
	while (n--)
		*(m++) = (u8)c;
	return s;
}