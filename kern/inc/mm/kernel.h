/*
 * mem/kernel.h
 * Kernel Memory properties
 */

#pragma once
#include <stdint.h>

extern uint32_t __kernel_start;
extern uint32_t __kernel_end;

#define kernel_start		((uintptr_t)&__kernel_start)
#define kernel_end			((uintptr_t)&__kernel_end)
