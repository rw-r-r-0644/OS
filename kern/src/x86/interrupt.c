#include <stdio.h>
#include <kernel/panic.h>
#include <x86/exception.h>
#include <x86/irq.h>
#include <x86/idt.h>
#include <x86/frame.h>
#include <x86/pic8259.h>

void interrupt_handler(struct interrupt_frame *frame)
{
	if (IS_IRQ(frame->interrupt))
		irq_handler(frame);
	else
		exception_handler(frame);
}

void interrupt_setup(void)
{
	pic8259_setup();
	idt_setup();
	irq_enable();
}
