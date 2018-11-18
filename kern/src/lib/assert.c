#include <assert.h>
#include <stdio.h>

void assert (int expression)
{
	if (expression == 0)
	{
		printf("\nFATAL: kernel assert() failed!!");
		printf("\nkernel halted.");
		for(;;);
	}
}