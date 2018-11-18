#include <unistd.h>
#include <dev/timer.h>

uint32_t usleep(uint32_t useconds)
{
	timer_wait(useconds / 1000); // Ehm.... (shitty way)
	return useconds;
}

uint32_t sleep(uint32_t seconds)
{
	timer_wait(1000 * seconds);
	return seconds;
}