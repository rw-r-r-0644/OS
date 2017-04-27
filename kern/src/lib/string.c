#include <string.h>

void *memchr(const void *s, int c, size_t n)
{
	while(n--)
		if(*((uint8_t *)s) ==(uint8_t)c)
			return(void *)s;
		else
			++s;
	return NULL;
}

int memcmp(const void *s1, const void *s2, size_t n)
{
	size_t i;
	for(i = 0; i < n &&((uint8_t*)s1)[i] ==((uint8_t*)s2)[i]; ++i);
	return(i == n) ? 0 :(((uint8_t*)s1)[i] -((uint8_t*)s2)[i]);
}

void *memcpy(void *s1, const void *s2, size_t n)
{
	while(n--)
		*((uint8_t *)s1++) = *((uint8_t *)s2++);
	return s1;
}

uint16_t *memcpy16(uint16_t *s1, const uint16_t *s2, size_t n)
{
	while(n--)
		*(s1++) = *(s2++);
	return s1;
}

void *memset(void *s, int c, size_t n)
{
	uint8_t* m =(uint8_t*)s;
	while(n--)
		*(m++) = (uint8_t)c;
	return s;
}

uint16_t *memset16(uint16_t *s, uint16_t v, size_t n)
{
	while(n--)
		*(s++) = v;
	return s;
}

char *strcat(char *s1, const char *s2)
{
	while(*++s1) ;
	while(*s2)
		*s1++ = *s2++;
	*s1++ = 0;
	return s1;
}

int strcmp(const char *s1, const char *s2)
{
	while(*s1 == *s2)
		if(*(s1++) == 0 || *(s2++) == 0) return 0;
	return *s1 - *s2;
}

char *strcpy(char *s1, const char *s2)
{
	while(*s2)
		*(s1++) = *(s2++);
	*(s1++) = 0;
	return s1;
}

size_t strlen(const char *s)
{
	size_t n = 0;
	while(*(s++)) ++n;
	return n;
}

char *strncat(char *s1, const char *s2, size_t n)
{
	while(*++s1) ;
	while(n-- && *s2)
		*s1++ = *s2++;
	*s1++ = 0;
	return s1;
}

char *strncpy(char *s1, const char *s2, size_t n)
{
	size_t i;
	for(i = 0; i < n && s2[i] != '\0'; i++)
        s1[i] = s2[i];
	if(i < n)
		memset(&s1[i], 0 , n - i);
	return s1;
}

char *strrchr(char * s, int c)
{
	for(size_t i = strlen(s); i != 0; i--)
		if(s[i] ==(char)c)
			return &s[i];
	return NULL;
}

void reverse(char *s)
{
    int c, i, j;
    for(i = 0, j = strlen(s)-1; i < j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}

