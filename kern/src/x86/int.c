#include <x86/cpu.h>
#include <utils/deadlock.h>
#include <utils/logger.h>
#include <kernel/utils.h>
#include <stdint.h>
#include <string.h>

/* IRQs */

void (*irq_hadlers[16])(regs_t *regs) = {};

void register_irq(unsigned irq, void (*handler)(regs_t *regs))
{
	irq_hadlers[irq] = handler;
}

void handle_irq(unsigned irq, regs_t *regs)
{
	if(irq >= 8)
		outb(0xA0, 0x20);
	outb(0x20, 0x20);

	if(irq_hadlers[irq])
		irq_hadlers[irq](regs);
}

void enable_irqs(void)
{
	outb(0x20, 0x11);
	outb(0xA0, 0x11);
	outb(0x21, 0x20);
	outb(0xA1, 0x28);
	outb(0x21, 0x04);
	outb(0xA1, 0x02);
	outb(0x21, 0x01);
	outb(0xA1, 0x01);
	outb(0x21, 0x00);
	outb(0xA1, 0x00);

	#define IRQG(n)	extern void irq##n(); idt_set_gate(n+32, irq##n)
	IRQG(0);IRQG(1);IRQG(2);IRQG(3);IRQG(4);IRQG(5);IRQG(6);IRQG(7);
	IRQG(8);IRQG(9);IRQG(10);IRQG(11);IRQG(12);IRQG(13);IRQG(14);IRQG(15);
}


/* ISRs */

void handle_isr(unsigned num, regs_t *regs)
{
	log_error("\n\nfatal exception %d: %s\n", num,
			(char*[]){
			"division by zero", "debug", "non maskable interrupt",
			"breakpoint", "overflow", "bound range exceeded",
			"invalid opcode", "device not available", "double fault",
			"coprocessor segment overrun", "invalid tss", "segment not present",
			"stack segment fault", "general protection fault", "page fault",
			"reserved", "x87 floating point exception", "alignment check",
			"machine check", "simd floating point exception",
			"virtualization exception" "reserved", "reserved", "reserved",
			"reserved", "reserved", "reserved", "reserved", "reserved",
			"reserved", "security exception", "reserved"
			}[num]
		);
	deadlock();
}

void enable_isrs(void)
{
	#define ISRG(n)	extern void isr##n(); idt_set_gate(n, isr##n)
	ISRG(0);ISRG(1);ISRG(2);ISRG(3);ISRG(4);ISRG(5);ISRG(6);ISRG(7);
	ISRG(8);ISRG(9);ISRG(10);ISRG(11);ISRG(12);ISRG(13);ISRG(14);ISRG(15);
	ISRG(16);ISRG(17);ISRG(18);ISRG(19);ISRG(20);ISRG(21);ISRG(22);ISRG(23);
	ISRG(24);ISRG(25);ISRG(26);ISRG(27);ISRG(28);ISRG(29);ISRG(30);ISRG(31);
}

/* IDT */

struct _packed
{
    uint16_t handl_lo;
    uint16_t segment;
    uint8_t zero;
    uint8_t flags;
    uint16_t handl_hi;
} idt[256];

struct _packed
{
    uint16_t limit;
    uint32_t base;
} idt_desc = { sizeof(idt) - 1, (uint32_t)idt };


void idt_init(void)
{
	memset(idt, 0, sizeof(idt));
	enable_isrs();
	enable_irqs();
	asm volatile("lidtl (%0)" : : "r" (&idt_desc));
}

void idt_set_gate(unsigned n, void (*handler)())
{
	idt[n].handl_lo = (uint16_t)((unsigned)handler & 0xFFFF);
	idt[n].segment = 0x08;
	idt[n].zero = 0x00;
	idt[n].flags = 0x8E;
	idt[n].handl_hi = (uint16_t)(((unsigned)handler >> 16) & 0xFFFF);
}
