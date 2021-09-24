#pragma once
#include <asm/irq.h>
#include <x86/pic8259.h>
#include <x86/frame.h>

#define IRQ_BASE     (PIC1_START_INTERRUPT)
#define IRQ_LAST     (PIC2_END_INTERRUPT)

#define IS_IRQ(intr) ((intr >= IRQ_BASE) && (intr <= IRQ_LAST))
#define IRQ(n)       (IRQ_BASE + (n))

#define IRQ_PIT      IRQ(0)
#define IRQ_KEYBOARD IRQ(1)
#define IRQ_COM2     IRQ(3)
#define IRQ_COM1     IRQ(4)
#define IRQ_LPT2     IRQ(5)
#define IRQ_FLOPPY   IRQ(6)
#define IRQ_LPT1     IRQ(7)
#define IRQ_CMOS     IRQ(8)
#define IRQ_MOUSE    IRQ(12)
#define IRQ_FPU      IRQ(13)
#define IRQ_ATA1     IRQ(14)
#define IRQ_ATA2     IRQ(15)


void
irq_handler(struct interrupt_frame *frame);
