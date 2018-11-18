/*
 * mem/kernel.h
 * Kernel Memory properties
 */

#pragma once
#include <stdint.h>

extern uint32_t p_kernel_start;
extern uint32_t p_kernel_end;

#define kernel_start		((uintptr_t)&p_kernel_start)
#define kernel_end			((uintptr_t)&p_kernel_end)
