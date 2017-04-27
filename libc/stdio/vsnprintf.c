#include <stdio.h>
#include <stdarg.h>
#include <string.h>

/*
 * Based on mini vsnprintf
 * Probably needs more checks
*/

int vsnprintf(char *buffer, u32 buffer_len, const char *fmt, va_list va)
{
	char *pbuffer = buffer;
	char bf[40];
	char ch;

	int append_char(char ch)
	{
		if ((u32)((pbuffer - buffer) + 1) >= buffer_len)
			return 0;
		*(pbuffer++) = ch;
		*(pbuffer) = '\0';
		return 1;
	}

	int append_string(char *s, u32 len)
	{
		u32 i;

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
		if ((u32)((pbuffer - buffer) + 1) >= buffer_len)
			break;
		
		if (ch!='%')
		{
			append_char(ch);
		}
		else
		{
			char zero_pad = 0;
			char *ptr;
			u32 len;

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
					len = int_to_ascii(va_arg(va, u32), 10, 0, (ch == 'u'), bf, zero_pad);
					append_string(bf, len);
					break;

				case 'p':
				case 'x':
				case 'X':
					len = int_to_ascii(va_arg(va, u32), 16, (ch == 'X'), 1, bf, zero_pad);
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


/*
int vsnprintf(char *buffer, u32 buffer_len, const char *fmt, va_list va)
{
	u32 len = 0;
	
	char *pbuffer = buffer;
	char bf[40];
	char ch;

	int append_char(char ch)
	{
		if ((u32)((pbuffer - buffer) + 1) >= buffer_len)
			return 0;
		*(pbuffer++) = ch;
		*(pbuffer) = '\0';
		return 1;
	}

	int append_string(char *s, u32 len)
	{
		u32 i;

		if (buffer_len - (pbuffer - buffer) - 1 < len)
			len = buffer_len - (pbuffer - buffer) - 1;

		// Copy to buffer
		for (i = 0; i < len; i++)
			*(pbuffer++) = s[i];
		*(pbuffer) = '\0';

		return len;
	}

	while ((ch = *(fmt++)))
	{	
		if (ch != '%')
		{
			if((len + 1) < buffer_len)
				buffer[len] = ch;
			len++;
		}
		else
		{
			char zero_pad = 0;
			char *ptr;
			u32 clen;

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
					clen = int_to_ascii(va_arg(va, u32), 10, 0, (ch == 'u'), bf, zero_pad);
					if((len + clen) < buffer_len)
						strcpy(&buffer[len], bf);
					len += clen;
					break;

				case 'p':
				case 'x':
				case 'X':
					len = int_to_ascii(va_arg(va, u32), 16, (ch == 'X'), 1, bf, zero_pad);
					append_string(bf, len);
					break;

				case 'c' :
					if((len + 1) < buffer_len)
						buffer[len] = (char)(va_arg(va, int));
					len++;
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
*/