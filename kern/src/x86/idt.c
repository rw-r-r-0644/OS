#include <stdint.h>
#include <asm/idt.h>
#include <x86/idt.h>
#include <x86/gdt.h>
#include <kernel/utils.h>

extern const uint32_t isr_table[], isr_count;

#define INTR_GATE_32 0x8e

struct idt_gate
{
	uint16_t offset_lo;
	uint16_t segment;
	uint8_t reserved;
	uint8_t flags;
	uint16_t offset_hi;
} _PACKED idt[256];

struct idt_desc
{
	uint16_t size;
	uint32_t offset;
} _PACKED idtr;

void idt_setup(void)
{
	/* prepare idt */
	for (unsigned long i = 0; i < isr_count; i++) {
		idt[i].offset_hi = isr_table[i] >> 16;
		idt[i].offset_lo = isr_table[i] & 0xffff;
		idt[i].segment = SEG_KERNEL_CODE;
		idt[i].flags = INTR_GATE_32;
	}

	/* load idt */
	lidt(idt, sizeof(idt) - 1);
}
