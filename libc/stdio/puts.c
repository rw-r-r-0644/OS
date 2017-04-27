#include <stdio.h>

#if defined(__is_libk)
#include <drivers/tty.h>
#endif

int puts(char* s)
{
	#if defined(__is_libk)
		// Kernel implementation
		tty_print(s);
	#else
		// TODO: Implement stdio and the write system call.
		s[0] = *s;
	#endif
	return 1;
}