#include <stdlib.h>
#include <string.h>

#if defined(__is_libk)
#include <mem/heap.h>
#endif
	
void * calloc(size_t num, size_t size)
{
#if defined(__is_libk)
	return memset(heap_alloc(&kernel_heap, num * size), 0, num * size);
#else
	// Not implemented yet.
	num = num;
	size = size;
	return NULL;
#endif
}