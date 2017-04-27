#include <stdio.h>

char * itoa(int value, char * str, int base)
{
	int_to_ascii(value, base, 0, 0, str, 0);
	return str;
}