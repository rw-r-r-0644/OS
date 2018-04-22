#pragma once

#define _CPU_X86_

// Include CPU headers
#include <arch/x86/gdt.h>
#include <arch/x86/idt.h>
#include <arch/x86/isr.h>
#include <arch/x86/ports.h>
#include <arch/x86/timer.h>

// Define some common asm functions
#define hlt() asm volatile("hlt")
#define sti() asm volatile("sti")
#define cli() asm volatile("cli")