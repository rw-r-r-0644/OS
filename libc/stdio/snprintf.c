#include <stdio.h>
#include <stdarg.h>
#include <string.h>

int snprintf(char* buffer, u32 buffer_len, const char *fmt, ...)
{
	int ret;
	va_list va;
	va_start(va, fmt);
	ret = vsnprintf(buffer, buffer_len, fmt, va);
	va_end(va);

	return ret;
}