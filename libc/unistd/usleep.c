#include <unistd.h>

#if defined(__is_libk)
#include <arch/x86/timer.h>
#endif

u32 usleep(u32 useconds)
{
#if defined(__is_libk)
	// Kernel implementation
	timer_wait(useconds / 1000); // Ehm.... (shitty way)
#else
	// TODO: Implement stdio and the write system call.
#endif
	return useconds;
}