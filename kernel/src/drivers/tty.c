#include <drivers/tty.h>
#include <arch/x86/ports.h>
#include <lib/stdint.h>
#include <lib/string.h>
#include <utils/utils.h>

#define TTY_COLS 80				// VGA default text mode cols
#define TTY_ROWS 25				// VGA default text mode rows
#define TTY_DEFAULT_COLOR 	// Default color: white on black
#define TTY_CTRL_REG 0x3D4
#define TTY_DATA_REG 0x3D5

u16 *tty_buffer = (u16*)0xB8000;
u16 tty_color = 0x0F00;

// Get cursor position from vga port as two bytes (data 0xE/0xF)
u32 tty_get_cursor()
{
	outb(TTY_CTRL_REG, 0xE);
	u32 offset = inb(TTY_DATA_REG) << 8;	// High byte
	outb(TTY_CTRL_REG, 0xF);
	offset += inb(TTY_DATA_REG);			// Low byte
	return offset;
}

// Set cursor position to offset
void tty_set_cursor(u32 offset)
{
	outb(TTY_CTRL_REG, 0xE);
	outb(TTY_DATA_REG, (u8)(offset >> 8));
	outb(TTY_CTRL_REG, 0xF);
	outb(TTY_DATA_REG, (u8)(offset & 0xFF));
}

// Create a char entry in the video buffer
void tty_print_char(char c, u16 color)
{
	if (color == 0) // No custom color has been specified, use default
		color = tty_color;
	
	u32 offset = tty_get_cursor(); // Get current offset from the cursor
	
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
		u16 shift = (TTY_ROWS - 1) * TTY_COLS;
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

void tty_print_color(char *s, u16 color)
{
	while(*s)
		tty_print_char(*(s++), color);
}

void tty_putchar_color(char c, u16 color)
{
	tty_print_char(c, color);
}

void tty_set_color(u16 color)
{
	tty_color = color;
}

void tty_clear()
{
	memset16(tty_buffer, ' ' | tty_color, TTY_COLS * TTY_ROWS);
	tty_set_cursor(0);
}
