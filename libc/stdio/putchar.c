#include <stdio.h>

#if defined(__is_libk)
#include <drivers/tty.h>
#endif

int putchar(int c)
{
#if defined(__is_libk)
	// Kernel implementation
	tty_putchar (c);
#else
	// TODO: Implement stdio and the write system call.
	return 1;
#endif
	return c;
}