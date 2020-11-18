#include <dev/kb.h>
#include <dev/timer.h>
#include <dev/tty.h>
#include <x86/cpu.h>

void dev_init()
{
	tty_init();

    // IRQ0: timer
    init_timer(1000);
	
    // IRQ1: keyboard
    init_kb();

    // Enable interruptions
    sti();
}
