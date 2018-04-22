#include <drivers/keyboard.h>
#include <arch/x86/cpu.h>
#include <init/main.h>

#include <utils/logger.h>

#include <stdio.h>
#include <string.h>
#include <stdint.h>


#define BACKSPACE 0x0E
#define ENTER 0x1C

static char key_buffer[256];

#define SC_MAX 57
const char *sc_name[] = { "ERROR", "Esc", "1", "2", "3", "4", "5", "6", 
    "7", "8", "9", "0", "-", "=", "Backspace", "Tab", "Q", "W", "E", 
        "R", "T", "Y", "U", "I", "O", "P", "[", "]", "Enter", "Lctrl", 
        "A", "S", "D", "F", "G", "H", "J", "K", "L", ";", "'", "`", 
        "LShift", "\\", "Z", "X", "C", "V", "B", "N", "M", ",", ".", 
        "/", "RShift", "Keypad *", "LAlt", "Spacebar"};

const char sc_ascii[] = { '?', '?', '1', '2', '3', '4', '5', '6',     
    '7', '8', '9', '0', '-', '=', '\b', '?', 'Q', 'W', 'E', 'R', 'T', 'Y', 
        'U', 'I', 'O', 'P', '[', ']', '?', '?', 'A', 'S', 'D', 'F', 'G', 
        'H', 'J', 'K', 'L', ';', '\'', '`', '?', '\\', 'Z', 'X', 'C', 'V', 
        'B', 'N', 'M', ',', '.', '/', '?', '?', '?', ' '};

const char sc_normal_ascii[] = { '?', '?', '1', '2', '3', '4', '5', '6',     
    '7', '8', '9', '0', '-', '=', '\b', '?', 'q', 'w', 'e', 'r', 't', 'y', 
        'u', 'i', 'o', 'p', '[', ']', '?', '?', 'a', 's', 'd', 'f', 'g', 
        'h', 'j', 'k', 'l', ';', '\'', '`', '\0', '\\', 'z', 'x', 'c', 'v', 
        'b', 'n', 'm', ',', '.', '/', '?', '?', '?', ' '};

const char sc_shift_ascii[] = { '?', '?', '!', '@', '#', '$', '%', '^',     
    '&', '*', '(', ')', '_', '+', '\b', '?', 'Q', 'W', 'E', 'R', 'T', 'Y', 
        'U', 'I', 'O', 'P', '[', ']', '?', '?', 'A', 'S', 'D', 'F', 'G', 
        'H', 'J', 'K', 'L', ':', '"', '`', '\0', '\\', 'Z', 'X', 'C', 'V', 
        'B', 'N', 'M', '<', '>', '?', '\0', '?', '?', ' '};

		
const char sc_maiusc_ascii[] = { '?', '?', '1', '2', '3', '4', '5', '6',     
    '7', '8', '9', '0', '-', '=', '\b', '?', 'Q', 'W', 'E', 'R', 'T', 'Y', 
        'U', 'I', 'O', 'P', '[', ']', '?', '?', 'A', 'S', 'D', 'F', 'G', 
        'H', 'J', 'K', 'L', ';', '\'', '`', '\0', '\\', 'Z', 'X', 'C', 'V', 
        'B', 'N', 'M', ',', '.', '/', '?', '?', '?', ' '};

int shift_state = 0;
		
static void keyboard_callback(registers_t *regs)
{
    // The PIC leaves the scancode in port 0x60
    u8 scancode = inb(0x60);
	
	switch(scancode)
	{
		// Shift press
		case 0x2A:
		case 0x36:
			shift_state = 1;
			break;
			
		// Shift release
		case 0xAA:
		case 0xB6:
			shift_state = 0;
			break;
			
		// Backspace
		case BACKSPACE: ;
			int len = strlen(key_buffer);
			if (len > 0) // Only delete if there's something to delete
			{
				key_buffer[len - 1] = '\0';
				putchar('\b');
			}
			break;
			
		// Enter
		case ENTER:
			putchar('\n');
			user_input(key_buffer); /* kernel-controlled function. this shouldn't work like this. */
			memset(key_buffer, '\0', 256);
			break;
		
		// Normal char
		default: ;
			char letter = (scancode > SC_MAX) ? '\0' : ((shift_state) ? sc_shift_ascii[(int)scancode] : sc_normal_ascii[(int)scancode]);
			append(key_buffer, letter);
			putchar(letter);
			break;
	}
}

void init_keyboard()
{
	register_interrupt_handler(IRQ1, keyboard_callback);
	log_printf(LOG_INFO, "setup keyboard irq");
}