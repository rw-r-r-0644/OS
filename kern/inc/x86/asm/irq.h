#pragma once

static inline unsigned long irqf_disable(void)
{
	unsigned long flags;
	asm volatile ("pushf\n\tcli\n\tpop %0" : "=r"(flags) : : "memory","cc");
	return flags;
}

static inline void irqf_restore(unsigned long flags)
{
	asm volatile ("push %0\n\tpopf" : : "rm"(flags) : "memory","cc");
}

static inline void irq_enable()
{
	asm volatile ("sti" : : : "cc");
}
