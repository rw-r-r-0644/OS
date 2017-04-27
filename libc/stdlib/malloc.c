#include <stdlib.h>

#if defined(__is_libk)
#include <mem/heap.h>
#endif
	
void * malloc(size_t size)
{
#if defined(__is_libk)
	return heap_alloc(&kernel_heap, size);
#else
	// Not implemented yet.
	size = size;
	return NULL;
#endif
}