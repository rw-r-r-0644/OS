#include <string.h>

char * strcat (char *s1, const char *s2)
{
	while (*++s1) ;
	while (*s2)
		*s1++ = *s2++;
	*s1++ = 0;
	return s1;
}