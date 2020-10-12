#include <lib/stdio.h>
#include <lib/utils.h>
#include <utils/deadlock.h>

_noreturn void
panic(char *msg)
{
	printf("*** Kernel panic: %s\n", msg);
	printf("*** System halted!\n");

	deadlock();
	__builtin_unreachable();
}
 
