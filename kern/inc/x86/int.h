#pragma once
#include <x86/regs.h>

void enable_irqs(void);
void handle_irq(unsigned irq, regs_t *regs);
void register_irq(unsigned irq, void (*handler)(regs_t *regs));

void enable_isrs(void);
void handle_isr(unsigned num, regs_t *regs);

void idt_init(void);
void idt_set_gate(unsigned n, void (*handler)());
