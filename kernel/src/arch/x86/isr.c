#include <arch/x86/cpu.h>

#include <stdio.h>
#include <string.h>

#include <drivers/keyboard.h>
#include <drivers/ide.h>
#include <drivers/tty.h>

#include <utils/logger.h>

void set_idt_gate(int n, u32 handler)
{
	idt[n].low_offset = (u16)((handler) & 0xFFFF);
	idt[n].sel = KERNEL_CS;
	idt[n].zero = 0;
	idt[n].flags = 0x8E;
	idt[n].high_offset = (u16)(((handler) >> 16) & 0xFFFF);
}

void set_idt()
{
	idt_reg.base = (u32)&idt;
	idt_reg.limit = IDT_ENTRIES * sizeof(idt_gate_t) - 1;
	// Don't make the mistake of loading &idt -- always load &idt_reg
	asm volatile("lidtl (%0)" : : "r" (&idt_reg));
}

isr_t interrupt_handlers[256];

/* Can't do this with a loop because we need the address
 * of the function names */
void isr_init()
{
    set_idt_gate(0, (u32)isr0);
    set_idt_gate(1, (u32)isr1);
    set_idt_gate(2, (u32)isr2);
    set_idt_gate(3, (u32)isr3);
    set_idt_gate(4, (u32)isr4);
    set_idt_gate(5, (u32)isr5);
    set_idt_gate(6, (u32)isr6);
    set_idt_gate(7, (u32)isr7);
    set_idt_gate(8, (u32)isr8);
    set_idt_gate(9, (u32)isr9);
    set_idt_gate(10, (u32)isr10);
    set_idt_gate(11, (u32)isr11);
    set_idt_gate(12, (u32)isr12);
    set_idt_gate(13, (u32)isr13);
    set_idt_gate(14, (u32)isr14);
    set_idt_gate(15, (u32)isr15);
    set_idt_gate(16, (u32)isr16);
    set_idt_gate(17, (u32)isr17);
    set_idt_gate(18, (u32)isr18);
    set_idt_gate(19, (u32)isr19);
    set_idt_gate(20, (u32)isr20);
    set_idt_gate(21, (u32)isr21);
    set_idt_gate(22, (u32)isr22);
    set_idt_gate(23, (u32)isr23);
    set_idt_gate(24, (u32)isr24);
    set_idt_gate(25, (u32)isr25);
    set_idt_gate(26, (u32)isr26);
    set_idt_gate(27, (u32)isr27);
    set_idt_gate(28, (u32)isr28);
    set_idt_gate(29, (u32)isr29);
    set_idt_gate(30, (u32)isr30);
    set_idt_gate(31, (u32)isr31);
	log_info("IDT gates set");

    // Remap the PIC
    outb(0x20, 0x11);
    outb(0xA0, 0x11);
    outb(0x21, 0x20);
    outb(0xA1, 0x28);
    outb(0x21, 0x04);
    outb(0xA1, 0x02);
    outb(0x21, 0x01);
    outb(0xA1, 0x01);
    outb(0x21, 0x0);
    outb(0xA1, 0x0);
	log_info("PIC remapped");

    // Install the IRQs
    set_idt_gate(32, (u32)irq0);
    set_idt_gate(33, (u32)irq1);
    set_idt_gate(34, (u32)irq2);
    set_idt_gate(35, (u32)irq3);
    set_idt_gate(36, (u32)irq4);
    set_idt_gate(37, (u32)irq5);
    set_idt_gate(38, (u32)irq6);
    set_idt_gate(39, (u32)irq7);
    set_idt_gate(40, (u32)irq8);
    set_idt_gate(41, (u32)irq9);
    set_idt_gate(42, (u32)irq10);
    set_idt_gate(43, (u32)irq11);
    set_idt_gate(44, (u32)irq12);
    set_idt_gate(45, (u32)irq13);
    set_idt_gate(46, (u32)irq14);
    set_idt_gate(47, (u32)irq15);
	log_info("IRQ installed");

    set_idt(); // Load with ASM
	log_info("IDT loaded");
}

/* To print the message which defines every exception */
char *exception_messages[] =
{
    "DIVISION_BY_ZERO_EXCEPTION",
    "DEBUG",
    "NON_MASKABLE_INTERRUPT_EXCEPTION",
    "BREAKPOINT",
    "INTO_DETECTED_OVERFLOW_EXCEPTION",
    "OUT_OF_BOUNDS_EXCEPTION",
    "INVALID_OPCODE_EXCEPTION",
    "NO_COPROCESSOR_EXCEPTION",

    "DOUBLE_FAULT_EXCEPTION",
    "COPROCESSOR_SEGMENT_OVERRUN_EXCEPTION",
    "BAD_TSS_EXCEPTION",
    "SEGMENT_NOT_PRESENT_EXCEPTION",
    "STACK_FAULT_EXCEPTION",
    "GENERAL_PROTECTION_FAULT_EXCEPTION",
    "PAGE_FAULT_EXCEPTION",
    "UNKNOWN_INTERRUPT_EXCEPTION",

    "COPROCESSOR_FAULT_EXCEPTION",
    "ALIGNMENT_CHECK_EXCEPTION",
    "MACHINE_CHECK_EXCEPTION",
    "RESERVED_EXCEPTION",
    "RESERVED_EXCEPTION",
    "RESERVED_EXCEPTION",
    "RESERVED_EXCEPTION",
    "RESERVED_EXCEPTION",

    "RESERVED_EXCEPTION",
    "RESERVED_EXCEPTION",
    "RESERVED_EXCEPTION",
    "RESERVED_EXCEPTION",
    "RESERVED_EXCEPTION",
    "RESERVED_EXCEPTION",
    "RESERVED_EXCEPTION",
    "RESERVED_EXCEPTION"
};

void isr_handler(registers_t *r)
{
	// BSOD irs template
	tty_set_color(vga_color(VGA_COLOR_LIGHT_BLUE, VGA_COLOR_WHITE));
	tty_clear();
	
	printf("OS\n\n");
	
	// Informations for user 
	printf("A problem has been detected and OS has\nbeen shutdown to prevent further damage.\nYou'll loose all your unsaved datas.\n\n");
	printf("%s\n\n", exception_messages[r->int_no]);
	
	// Technical informations
	printf("Technical informations:\n");
	
	switch(r->int_no)
	{
		case 14: ; // Page fault
			u32 faulting_address;
			asm volatile("mov %%cr2, %0" : "=r" (faulting_address)); // the faulting address is stored in CR2
			
			// Print exception additional informations
			printf("page fault info:\n");
			printf("address:  0x%X\n", faulting_address);
			printf("present:  %u\n", r->err_code & 0x1);	// Page not present
			printf("rw:       %u\n", r->err_code & 0x2);	// Write operation?
			printf("user:     %u\n", r->err_code & 0x4);	// Processor was in user-mode?
			printf("reserved: %u\n", r->err_code & 0x8);	// Overwritten CPU-reserved bits of page entry?
			printf("id:       %u\n", r->err_code & 0x10);	// Caused by an instruction fetch?
			break;
		
		default: ; // Generic exception
			printf("EXCEPTION:  %08X\n", r->int_no);
			printf("ERROR_CODE: %08X\n", r->err_code);
			
			// Registers dump
			printf("REGISTERS:\n");
			printf("ds =%08X edi=%08X esi=%08X ebp=%08X\nebx=%08X edx=%08X ecx=%08X eax=%08X\neip=%08X cs =%08X flg=%08X esp=%08X\nss =%08X\n",
					r->ds,  r->edi, r->esi,    r->ebp,
					r->ebx, r->edx, r->ecx,    r->eax,
					r->eip, r->cs,  r->eflags, r->esp,
					r->ss);
			break;
	}
			
	// Stop the cpu
	for(;;);
}

void register_interrupt_handler(u8 n, isr_t handler)
{
    interrupt_handlers[n] = handler;
}

void irq_handler(registers_t *r)
{
    /* After every interrupt we need to send an EOI to the PICs
     * or they will not send another interrupt again */
    if (r->int_no >= 40) outb(0xA0, 0x20); /* slave */
    outb(0x20, 0x20); /* master */

    /* Handle the interrupt in a more modular way */
    if (interrupt_handlers[r->int_no] != 0)
	{
        isr_t handler = interrupt_handlers[r->int_no];
        handler(r);
    }
}

void irq_init()
{
    // Enable interruptions
    sti();
	
    // IRQ0: timer
    init_timer(1000);
	
    // IRQ1: keyboard
    init_keyboard();
	
	// IRQ14/15: ide
	init_ide_irq();
}
