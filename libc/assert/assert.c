#include <assert.h>
#include <stdio.h>

void assert (int expression)
{
	if (expression == 0)
	{
		#if defined(__is_libk)
			printf("\nFATAL: kernel assert() failed!!");
			printf("\nkernel halted.");
			for(;;);
		#else
			// TODO: Properly terminate userspace program
		#endif
	}
}