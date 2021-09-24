#include <asm/io.h>
#include <x86/pic8259.h>
#include <kernel/utils.h>

#define PIC1_PORT_A 0x20
#define PIC1_PORT_B 0x21
#define PIC2_PORT_A 0xA0
#define PIC2_PORT_B 0xA1

#define PIC_ACK     0x20

void pic8259_acknowledge(unsigned irq)
{
	if (irq < PIC1_START_INTERRUPT || irq > PIC2_END_INTERRUPT)
		return;

	outb(PIC1_PORT_A, PIC_ACK);

	if (irq >= PIC2_START_INTERRUPT)
		outb(PIC2_PORT_A, PIC_ACK);
}

void pic8259_setup()
{
	/* remap PIC interrupts */
	outb(PIC1_PORT_A, 0x11); // ICW1
	outb(PIC2_PORT_A, 0x11);

	outb(PIC1_PORT_B, PIC1_START_INTERRUPT); // ICW2
	outb(PIC2_PORT_B, PIC2_START_INTERRUPT);

	outb(PIC1_PORT_B, 0x04); // ICW3
	outb(PIC2_PORT_B, 0x02);

	outb(PIC1_PORT_B, 0x01); // ICW4
	outb(PIC2_PORT_B, 0x01);

	outb(PIC1_PORT_B, 0x00);
	outb(PIC2_PORT_B, 0x00);
}
