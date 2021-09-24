#pragma once
#include <stdint.h>
#include <kernel/utils.h>

static inline void lidt(void* base, uint16_t limit)
{
	struct
	{
		uint16_t limit;
		void* base;
	} _PACKED idtr = { limit, base };

	asm volatile ( "lidt %0" : : "m"(idtr) );
}
