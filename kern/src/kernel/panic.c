#include <stdio.h>
#include <kernel/panic.h>
#include <kernel/utils.h>

_NORETURN void
panic(char *msg)
{
	printf("*** Kernel panic: %s\n", msg);
	printf("*** System halted!\n");

	deadlock();
	__builtin_unreachable();
}
 
