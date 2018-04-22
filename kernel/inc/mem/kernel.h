/*
 * mem/kernel.h
 * Kernel Memory properties
 */

#pragma once
#include <stdint.h>

extern u32 p_kernel_start;
extern u32 p_kernel_end;

#define kernel_start		((uintptr_t)&p_kernel_start)
#define kernel_end			((uintptr_t)&p_kernel_end)
