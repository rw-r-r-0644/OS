#include <string.h>

int memcmp (const void *s1, const void *s2, size_t n)
{
	size_t i;
	for (i = 0; i < n && ((u8*)s1)[i] == ((u8*)s2)[i]; ++i);
	return (i == n) ? 0 : (((u8*)s1)[i] - ((u8*)s2)[i]);
}