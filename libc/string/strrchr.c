#include <string.h>

char * strrchr(char * s, int c)
{
	for(size_t i = strlen(s); i != 0; i--)
		if (s[i] == (char)c)
			return &s[i];
	return NULL;
}