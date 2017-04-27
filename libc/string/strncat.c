#include <string.h>

char * strncat (char *s1, const char *s2, size_t n)
{
	while (*++s1) ;
	while (n-- && *s2)
		*s1++ = *s2++;
	*s1++ = 0;
	return s1;
}