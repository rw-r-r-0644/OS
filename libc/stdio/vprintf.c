#include <stdio.h>
#include <stdarg.h>
#include <string.h>

int vprintf(const char *fmt, va_list va)
{
	u32 len = 0;
	
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
					len += int_to_ascii(va_arg(va, u32), 10, 0, (ch == 'u'), bf, zero_pad);
					puts(bf);
					break;

				case 'p':
				case 'x':
				case 'X':
					len += int_to_ascii(va_arg(va, u32), 16, (ch == 'X'), 1, bf, zero_pad);
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