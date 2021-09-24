#include <stdio.h>
#include <asm/io.h>

#define KBD_DATA_PORT   0x60

unsigned char read_scan_code(void) {
	return inb(KBD_DATA_PORT);
}

void keyboard_interrupt(void) {
	printf("received scancode: %d\n", read_scan_code());
}
