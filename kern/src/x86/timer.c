#include <asm/io.h>
#include <x86/timer.h>

#define TIMER_HWCLOCK  (1193180ul)
#define TIMER_CLOCK    (1000ul)

#define TIMER_CLOCKDIV (TIMER_HWCLOCK / TIMER_CLOCK)

volatile uint32_t ticks;

void timer_interrupt(void)
{
	ticks++;
}

uint32_t timer_get_ticks()
{
	return ticks;
}

void timer_init()
{
	outb(0x43, 0x36);
	outb(0x40, TIMER_CLOCKDIV & 0xFF);
	outb(0x40, TIMER_CLOCKDIV >> 8);
	ticks = 0;
}

