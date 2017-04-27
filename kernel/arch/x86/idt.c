#include <arch/x86/idt.h>
#include <stdint.h>

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
    idt_reg.base = (u32) &idt;
    idt_reg.limit = IDT_ENTRIES * sizeof(idt_gate_t) - 1;
    // Don't make the mistake of loading &idt -- always load &idt_reg
    asm volatile("lidtl (%0)" : : "r" (&idt_reg));
}
