#pragma once
#include <kernel/utils.h>

struct interrupt_frame
{
	unsigned int eax;
	unsigned int ebx;
	unsigned int ecx;
	unsigned int edx;
	unsigned int esi;
	unsigned int edi;
	unsigned int ebp;
	unsigned int interrupt;
	unsigned int error_code;
	unsigned int eip;
	unsigned int cs;
	unsigned int eflags;
	unsigned int esp;
	unsigned int ss;
} _PACKED;
