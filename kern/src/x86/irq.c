#include <x86/irq.h>
#include <x86/keyboard.h>
#include <x86/timer.h>

void irq_handler(struct interrupt_frame *frame)
{
	if (frame->interrupt == IRQ_KEYBOARD)
		keyboard_interrupt();
	else if (frame->interrupt == IRQ_PIT)
		timer_interrupt();

	pic8259_acknowledge(frame->interrupt);
}
