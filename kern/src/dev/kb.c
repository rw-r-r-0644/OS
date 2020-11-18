#include <dev/kb.h>
#include <x86/cpu.h>
#include <kernel/main.h>

#include <utils/logger.h>

#include <stdio.h>
#include <string.h>
#include <stdint.h>

static char in_buf[256];
static int in_len = 0;

enum sc_keys
{
	SC_LSHIFT_DOWN = 0x2A,
	SC_RSHIFT_DOWN = 0x36,
	SC_LSHIFT_UP = 0xAA,
	SC_RSHIFT_UP = 0xB6,
	SC_BACKSPACE_DOWN = 0x0E,
	SC_ENTER_DOWN = 0x1C,
	SC_CAPSLOCK_DOWN = 0x3A,
};

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

int in_shift = 0;
int in_maiusc = 0;

static void kb_irq(regs_t *regs)
{
    uint8_t sc = inb(0x60);
	char ch;
	switch(sc)
	{
		case SC_LSHIFT_DOWN:
		case SC_RSHIFT_DOWN:
			in_shift = 1;
			break;
		case SC_LSHIFT_UP:
		case SC_RSHIFT_UP:
			in_shift = 0;
			break;
		case SC_CAPSLOCK_DOWN:
			in_maiusc = !in_maiusc;
			break;
		case SC_BACKSPACE_DOWN:
			if(in_len)
			{
				in_len--;
				putchar('\b');
			}
			break;
		case SC_ENTER_DOWN:
			putchar('\n');
			in_buf[in_len] = '\0';
			user_input(in_buf);
			in_len = 0;
			break;
		default:
			if(sc > SC_MAX)
				break;
			if(in_shift)
				ch = sc_shift_ascii[sc];
			else if(in_maiusc)
				ch = sc_maiusc_ascii[sc];
			else
				ch = sc_normal_ascii[sc];
			in_buf[in_len++] = ch;
			putchar(ch);
			break;
	}
}

void init_kb()
{
	register_irq(1, kb_irq);
	log_info("setup keyboard irq");
}
