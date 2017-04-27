#include <stdlib.h>
#include <stddef.h>

int atoi (const char * s)
{
	if (s == NULL)
		return 0;

	int res = 0;
	int sign = 1;

	if (*s == '-') // < 0
	{
		sign = -1;
		s++;
	}
	while (*s != '\0' && *s >= '0' && *s <= '9')
		res = res * 10 + (*(s++) - '0');
  
	return sign * res;
}