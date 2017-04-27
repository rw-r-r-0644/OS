#include <string.h>

char * strcpy (char *s1, const char *s2)
{
	while (*s2)
		*(s1++) = *(s2++);
	*(s1++) = 0;
	return s1;
}
