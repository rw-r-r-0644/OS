#include <stdlib.h>

#if defined(__is_libk)
#include <mem/heap.h>
#endif
	
void free(void* ptr)
{
#if defined(__is_libk)
	heap_free(&kernel_heap, ptr);
	return;
#else
	// Not implemented yet.
	ptr = ptr;
#endif
}