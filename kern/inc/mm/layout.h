/*
 * mm/layout.h
 * 
 * Memory layout constants
 */
#pragma once

extern unsigned char __kernel_start[];
extern unsigned char __kernel_end[];

#define KERNEL_START		((unsigned long)__kernel_start)
#define KERNEL_END			((unsigned long)__kernel_end)
#define KERNEL_SIZE			(KERNEL_END - KERNEL_START)

#define KERNEL_VIRT_OFFSET	0xC0000000
#define KERNEL_PA_START		(KERNEL_START - KERNEL_VIRT_OFFSET)
#define KERNEL_PA_END		(KERNEL_END - KERNEL_VIRT_OFFSET)

