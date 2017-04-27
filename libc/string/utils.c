#include <string.h>

void reverse(char *s)
{
    int c, i, j;
    for (i = 0, j = strlen(s)-1; i < j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}

void append(char *s, char n)
{
    int len = strlen(s);
    s[len] = n;
    s[len+1] = '\0';
}