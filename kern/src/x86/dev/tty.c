#include <dev/tty.h>
#include <x86/cpu.h>
#include <lib/stdint.h>
#include <lib/string.h>
#include <kernel/utils.h>
#include <kernel/resources.h>
#include <kernel/panic.h>
#include <mm/mm.h>

#define TTY_COLS 80				// VGA default text mode cols
#define TTY_ROWS 25				// VGA default text mode rows
#define TTY_CTRL_REG 0x3D4
#define TTY_DATA_REG 0x3D5

uint16_t *tty_buffer;
uint16_t tty_color = 0x0F00;
static int tty_is_init = 0;

// Get cursor position from vga port as two bytes (data 0xE/0xF)
uint32_t tty_get_cursor()
{
	outb(TTY_CTRL_REG, 0xE);
	uint32_t offset = inb(TTY_DATA_REG) << 8;	// High byte
	outb(TTY_CTRL_REG, 0xF);
	offset += inb(TTY_DATA_REG);			// Low byte
	return offset;
}

// Set cursor position to offset
void tty_set_cursor(uint32_t offset)
{
	outb(TTY_CTRL_REG, 0xE);
	outb(TTY_DATA_REG, (uint8_t)(offset >> 8));
	outb(TTY_CTRL_REG, 0xF);
	outb(TTY_DATA_REG, (uint8_t)(offset & 0xFF));
}

// Create a char entry in the video buffer
void tty_print_char(char c, uint16_t color)
{
	if (!tty_is_init)
		return;
	if (color == 0) // No custom color has been specified, use default
		color = tty_color;
	
	uint32_t offset = tty_get_cursor(); // Get current offset from the cursor
	
	switch(c)
	{
		case '\n':	// New line
			offset = (offset + TTY_COLS) - (offset % TTY_COLS);
			break;
		case '\t':	// Tab
			offset = (offset + 4) - (offset % 4);
			break;
		case '\b':	// Backspace
			tty_buffer[--offset] = ' ' | color;
			break;
		case '\0':	// NULL char
			return;
		default:	// Normal char
			tty_buffer[offset++] = c | color;
			break;
	}

	// Check if scrolling is needed
	if (offset >= (TTY_ROWS * TTY_COLS))
	{
		uint16_t shift = (TTY_ROWS - 1) * TTY_COLS;
		memcpy16(tty_buffer, &tty_buffer[TTY_COLS], shift);
		memset16(&tty_buffer[shift], ' ' | tty_color, TTY_COLS);
		
		offset -= TTY_COLS;
	}

	tty_set_cursor(offset);
}

void tty_print(char *s)
{
	while(*s)
		tty_print_char(*(s++), 0);
}

void tty_putchar(char c)
{
	tty_print_char(c, 0);
}

void tty_print_color(char *s, uint16_t color)
{
	while(*s)
		tty_print_char(*(s++), color);
}

void tty_putchar_color(char c, uint16_t color)
{
	tty_print_char(c, color);
}

void tty_set_color(uint16_t color)
{
	tty_color = color;
}

void tty_clear()
{
	memset16(tty_buffer, ' ' | tty_color, TTY_COLS * TTY_ROWS);
	tty_set_cursor(0);
}

void tty_init()
{
	resource *tbuf;

	tbuf = iomemrequest(0xB8000, 
		TTY_COLS * TTY_ROWS * 2,
		"Text mode buffer"
	);

	if (!tbuf)
		panic("tty_init: failed to claim memory");

	tty_buffer = ioremap(tbuf);
	
	tty_is_init = 1;
}
