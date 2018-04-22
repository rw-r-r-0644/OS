#include <unistd.h>
#include <arch/x86/timer.h>

u32 usleep(u32 useconds)
{
	timer_wait(useconds / 1000); // Ehm.... (shitty way)
	return useconds;
}

u32 sleep(u32 seconds)
{
	timer_wait(1000 * seconds);
	return seconds;
}