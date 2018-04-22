#include <arch/x86/timer.h>
#include <arch/x86/cpu.h>

#include <utils/logger.h>

volatile u32 tick;

static void timer_callback(registers_t *regs)
{
    tick++;
}

u32 timer_get_ticks()
{
	return tick;
}

void timer_wait(u32 ticks)
{
	sti(); // Enable interrupts
	tick = 0;
	while(tick < ticks); // Wait for ticks to get to the amount of time
}

void init_timer(u32 freq) {
    /* Install the function we just wrote */
    register_interrupt_handler(IRQ0, timer_callback);

    /* Get the PIT value: hardware clock at 1193180 Hz */
    u32 divisor = 1193180 / freq;
    u8 low  = (u8)(divisor & 0xFF);
    u8 high = (u8)( (divisor >> 8) & 0xFF);
    /* Send the command */
    outb(0x43, 0x36); /* Command port */
    outb(0x40, low);
    outb(0x40, high);

	log_printf(LOG_INFO, "init at freq %u hz", freq);
}

