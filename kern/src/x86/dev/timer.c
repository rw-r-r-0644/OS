#include <dev/timer.h>
#include <x86/cpu.h>

#include <utils/logger.h>

volatile uint32_t tick;

static void timer_callback(regs_t *regs)
{
    tick++;
}

uint32_t timer_get_ticks()
{
	return tick;
}

void timer_wait(uint32_t ticks)
{
	sti(); // Enable interrupts
	tick = 0;
	while(tick < ticks); // Wait for ticks to get to the amount of time
}

void init_timer(uint32_t freq) {
    /* Install the function we just wrote */
    register_irq(0, timer_callback);

    /* Get the PIT value: hardware clock at 1193180 Hz */
    uint32_t divisor = 1193180 / freq;
    uint8_t low  = (uint8_t)(divisor & 0xFF);
    uint8_t high = (uint8_t)( (divisor >> 8) & 0xFF);
    /* Send the command */
    outb(0x43, 0x36); /* Command port */
    outb(0x40, low);
    outb(0x40, high);

	log_info("init at freq %u hz", freq);
}

