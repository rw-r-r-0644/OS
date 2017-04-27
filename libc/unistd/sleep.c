#include <unistd.h>

#if defined(__is_libk)
#include <arch/x86/timer.h>
#endif

u32 sleep(u32 seconds)
{
#if defined(__is_libk)
	// Kernel implementation
	timer_wait(1000 * seconds);
#else
	// TODO: Implement stdio and the write system call.
#endif
	return seconds;
}