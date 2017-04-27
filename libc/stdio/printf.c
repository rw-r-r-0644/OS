#include <stdio.h>
#include <stdarg.h>
#include <string.h>

void printf(const char *fmt, ...)
{
	va_list va;
	va_start(va,fmt);
	vprintf(fmt, va);
	va_end(va);
}