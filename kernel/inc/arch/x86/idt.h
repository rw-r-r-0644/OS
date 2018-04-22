/*
 * idt.h
 *
 * Interrupt descriptor table functions and definitions
*/

#pragma once

#include <lib/stdint.h>
#include <lib/utils.h>

// Segment selectors
#define KERNEL_CS 0x08

// How every interrupt gate (handler) is defined
typedef struct
{
    u16 low_offset;		// Lower 16 bits of handler function address
    u16 sel;			// Kernel segment selector
    u8  zero;			// Must be zero

    /* First byte
       Bit 7: "Interrupt is present"
       Bits 6-5: Privilege level of caller (0=kernel..3=user)
       Bit 4: Set to 0 for interrupt gates
       Bits 3-0: bits 1110 = decimal 14 = "32 bit interrupt gate" */
    u8 flags;
    u16 high_offset;	// Higher 16 bits of handler function address */
} _packed idt_gate_t;

// Struct passed to the lidt assembly instruction
typedef struct
{
    u16 limit;	// IDT size
    u32 base;	// IDT pointer
} _packed idt_register_t;

#define IDT_ENTRIES 256
idt_gate_t idt[IDT_ENTRIES];
idt_register_t idt_reg;

//! Setup an handler for the n interrupt
void set_idt_gate(int n, u32 handler);

//! Set the idt
void set_idt();