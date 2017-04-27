#pragma once

typedef struct
{
	unsigned edi, esi, ebp, esp, ebx, edx, ecx, eax;
	unsigned err;
	unsigned eip, cs, eflags, useresp, ss;
} regs_t;

static inline unsigned get_eflags(void)
{
	unsigned eflags;
	asm volatile("pushfl; popl %0" : "=r" (eflags));
	return eflags;
}

static inline unsigned get_cr3(void)
{
	unsigned cr3;
	asm volatile("movl %%cr3, %%eax; movl %%eax, %0;":"=m"(cr3)::"%eax");
	return cr3;
}
	
