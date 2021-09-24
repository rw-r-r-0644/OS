#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <x86/tty.h>

uint32_t int_to_ascii(int value, uint32_t radix, uint32_t uppercase, uint32_t unsig, char *buffer, uint32_t zero_pad)
{
	char *pbuffer = buffer;
	int	negative = 0;
	uint32_t i, len;

	/* No support for unusual radixes. */
	if (radix > 16)
		return 0;

	if (value < 0 && !unsig) {
		negative = 1;
		value = -value;
	}

	/* This builds the string back to front ... */
	do {
		int digit = value % radix;
		*(pbuffer++) = (digit < 10 ? '0' + digit : (uppercase ? 'A' : 'a') + digit - 10);
		value /= radix;
	} while (value > 0);

	for (i = (pbuffer - buffer); i < zero_pad; i++)
		*(pbuffer++) = '0';

	if (negative)
		*(pbuffer++) = '-';

	*(pbuffer) = '\0';

	/* ... now we reverse it (could do it recursively but will
	 * conserve the stack space) */
	len = (pbuffer - buffer);
	for (i = 0; i < len / 2; i++) {
		char j = buffer[i];
		buffer[i] = buffer[len-i-1];
		buffer[len-i-1] = j;
	}

	return len;
}

char * itoa(int value, char * str, int base)
{
	int_to_ascii(value, base, 0, 0, str, 0);
	return str;
}

void printf(const char *fmt, ...)
{
	va_list va;
	va_start(va,fmt);
	vprintf(fmt, va);
	va_end(va);
}

int putchar(int c)
{
	tty_putchar (c);
	return c;
}

int puts(char* s)
{
	tty_print(s);
	return 1;
}

int snprintf(char* buffer, uint32_t buffer_len, const char *fmt, ...)
{
	int ret;
	va_list va;
	va_start(va, fmt);
	ret = vsnprintf(buffer, buffer_len, fmt, va);
	va_end(va);

	return ret;
}


int vprintf(const char *fmt, va_list va)
{
	uint32_t len = 0;
	
	char bf[40];
	char ch;

	while ((ch = *(fmt++)))
	{
		if (ch != '%')
		{
			len++;
			putchar(ch);
		}
		else
		{
			char zero_pad = 0;
			char *ptr;

			ch = *(fmt++);

			if(ch == '0') // Zero padding
			{
				ch = *(fmt++);
				if(ch == '\0')
					goto end;
				if(ch >= '0' && ch <= '9')
					zero_pad = ch - '0';
				ch = *(fmt++);
			}

			if (ch == 'l' && (*(fmt) == 'u' || *(fmt) == 'd'))
				ch = *(fmt++);
			
			switch(ch)
			{
				case 0:
					goto end;

				case 'u':
				case 'd':
					len += int_to_ascii(va_arg(va, uint32_t), 10, 0, (ch == 'u'), bf, zero_pad);
					puts(bf);
					break;

				case 'p':
				case 'x':
				case 'X':
					len += int_to_ascii(va_arg(va, uint32_t), 16, (ch == 'X'), 1, bf, zero_pad);
					puts(bf);
					break;

				case 'c' :
					len++;
					putchar((char)(va_arg(va, int)));
					break;

				case 's' :
					ptr = va_arg(va, char*);
					len += strlen(ptr);
					puts(ptr);
					break;

				default:
					len++;
					putchar(ch);
					break;
			}
		}
	}
end:
	return len;
}


/*
 * Based on mini vsnprintf
 * Probably needs more checks
*/

int vsnprintf(char *buffer, uint32_t buffer_len, const char *fmt, va_list va)
{
	char *pbuffer = buffer;
	char bf[40];
	char ch;

	int append_char(char ch)
	{
		if ((uint32_t)((pbuffer - buffer) + 1) >= buffer_len)
			return 0;
		*(pbuffer++) = ch;
		*(pbuffer) = '\0';
		return 1;
	}

	int append_string(char *s, uint32_t len)
	{
		uint32_t i;

		if (buffer_len - (pbuffer - buffer) - 1 < len)
			len = buffer_len - (pbuffer - buffer) - 1;

		/* Copy to buffer */
		for (i = 0; i < len; i++)
			*(pbuffer++) = s[i];
		*(pbuffer) = '\0';

		return len;
	}

	while ((ch = *(fmt++)))
	{
		if ((uint32_t)((pbuffer - buffer) + 1) >= buffer_len)
			break;
		
		if (ch!='%')
		{
			append_char(ch);
		}
		else
		{
			char zero_pad = 0;
			char *ptr;
			uint32_t len;

			ch = *(fmt++);

			if(ch == '0') // Zero padding
			{
				ch = *(fmt++);
				if(ch == '\0')
					goto end;
				if(ch >= '0' && ch <= '9')
					zero_pad = ch - '0';
				ch = *(fmt++);
			}

			if (ch == 'l' && (*(fmt) == 'u' || *(fmt) == 'd'))
				ch = *(fmt++);
			
			switch(ch)
			{
				case 0:
					goto end;

				case 'u':
				case 'd':
					len = int_to_ascii(va_arg(va, uint32_t), 10, 0, (ch == 'u'), bf, zero_pad);
					append_string(bf, len);
					break;

				case 'p':
				case 'x':
				case 'X':
					len = int_to_ascii(va_arg(va, uint32_t), 16, (ch == 'X'), 1, bf, zero_pad);
					append_string(bf, len);
					break;

				case 'c' :
					append_char((char)(va_arg(va, int)));
					break;

				case 's' :
					ptr = va_arg(va, char*);
					append_string(ptr, strlen(ptr));
					break;

				default:
					append_char(ch);
					break;
			}
		}
	}
end:
	return pbuffer - buffer;
}
