#ifndef _I386_PIC_H
#define _I386_PIC_H

#define PIC1_START_INTERRUPT 0x20
#define PIC2_START_INTERRUPT 0x28
#define PIC2_END_INTERRUPT   (PIC2_START_INTERRUPT + 7)

void pic8259_acknowledge(unsigned int irq);
void pic8259_setup();

#endif
