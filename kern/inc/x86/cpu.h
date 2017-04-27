#pragma once

#define _CPU_X86_

// Include CPU headers
#include <x86/io.h>
#include <x86/int.h>
#include <x86/regs.h>
#include <dev/timer.h>

// Define some common asm functions
#define hlt() asm volatile("hlt")
#define sti() asm volatile("sti")
#define cli() asm volatile("cli")
