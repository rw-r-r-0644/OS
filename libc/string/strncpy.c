#include <string.h>

char * strncpy (char *s1, const char *s2, size_t n)
{
	size_t i;
	for (i = 0; i < n && s2[i] != '\0'; i++)
        s1[i] = s2[i];
	if (i < n)
		memset(&s1[i], 0 , n - i);
	return s1;
}