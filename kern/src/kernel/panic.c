#include <lib/stdio.h>
#include <kernel/utils.h>
#include <utils/deadlock.h>

_NORETURN void
panic(char *msg)
{
	printf("*** Kernel panic: %s\n", msg);
	printf("*** System halted!\n");

	deadlock();
	__builtin_unreachable();
}
 
