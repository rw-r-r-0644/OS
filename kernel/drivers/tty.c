/*
 * TTY text mode screen library
 * Outputs text to vga text framebuffer
 * For functions descriptions look at tty.h
*/

#include <drivers/tty.h>
#include <mem/kmalloc.h>
#include <arch/x86/ports.h>

#include <stdint.h>
#include <string.h>

// TTY definitions
#define TTY_BASE_ADDR 0xB8000	// Default adress for VGA text mode buffer
#define TTY_COLS 80				// VGA default text mode cols
#define TTY_ROWS 25				// VGA default text mode rows
#define TTY_DEFAULT_COLOR 0x0F	// Default color: white on black

// TTY I/O ports
#define TTY_CTRL_REG 0x3D4
#define TTY_DATA_REG 0x3D5

// Globals
u8 tty_color;
u8 *tty_video_buffer;

/// Private functions

int get_offset(int col, int row) { return 2 * (row * TTY_COLS + col); }
int get_offset_row(int offset) { return offset / (2 * TTY_COLS); }
int get_offset_col(int offset) { return (offset - (get_offset_row(offset)*2*TTY_COLS))/2; }

// Get cursor position from vga port as two bytes (data 0xE/0xF)
u32 tty_get_cursor_offset()
{
	outb(TTY_CTRL_REG, 0xE);
	u32 offset = inb(TTY_DATA_REG) << 8;	// High byte
	outb(TTY_CTRL_REG, 0xF);
	offset += inb(TTY_DATA_REG);			// Low byte
	return offset * 2;						// Each vga entry is two bytes
}

// Set cursor position to offset
void tty_set_cursor_offset(u32 offset)
{
	offset /= 2;
	outb(TTY_CTRL_REG, 0xE);
	outb(TTY_DATA_REG, (u8)(offset >> 8));
	outb(TTY_CTRL_REG, 0xF);
	outb(TTY_DATA_REG, (u8)(offset & 0xFF));
}

// Create a char entry in the video buffer
void tty_print_char(char c, char color)
{
	if (color == 0x00) // No custom color has been specified, use default
		color = tty_color;
	
	u32 offset = tty_get_cursor_offset(); // Get current offset from the cursor
	
	// Choose what to do with the char
	switch(c)
	{
		case '\n':	// New line
			offset = get_offset(0, get_offset_row(offset) + 1);
			break;
		case '\b':	// Backspace
			offset -= 2;
			tty_video_buffer[offset] = ' ';
			tty_video_buffer[offset + 1] = color;
			break;
		case '\0':	// NULL char
			return;
		default:	// Normal char
			tty_video_buffer[offset] = c;
			tty_video_buffer[offset + 1] = color;
			offset += 2;
			break;
	}
	
	// Check if scrolling is needed
	if (offset >= TTY_ROWS * TTY_COLS * 2)
	{
		memcpy(tty_video_buffer, &tty_video_buffer[TTY_COLS * 2], (TTY_ROWS - 1) * TTY_COLS * 2);
		// Clear the last line
		u32 last_line = 2 * ((TTY_ROWS - 1) * TTY_COLS);
		for (u32 i = 0; i < (TTY_COLS * 2); i += 2)
		{
			tty_video_buffer[i + last_line] = 0;
			tty_video_buffer[i + last_line + 1] = tty_color;
		}
		
		offset -= 2 * TTY_COLS;
	}

	tty_set_cursor_offset(offset);
}

/// Public functions

void tty_print(char *s)
{
	while(*s)
		tty_print_char(*(s++), 0);
}

void tty_putchar(char c)
{
	tty_print_char(c, 0);
}

void tty_print_color(char *s, vga_color_t color)
{
	char color_c = ((u8)color.bg << 1) | (u8)color.fg;
	while(*s)
		tty_print_char(*(s++), color_c);
}

void tty_putchar_color(char c, vga_color_t color)
{
	char color_c = ((u8)color.bg << 1) | (u8)color.fg;
	tty_print_char(c, color_c);
}

void tty_set_color(vga_color_t color)
{
	tty_color = ((u8)color.bg << 1) | (u8)color.fg;
}

void tty_clear_screen()
{
	int screen_size = TTY_COLS * TTY_ROWS * 2;
	for (int i = 0; i < screen_size; i += 2)
	{
		tty_video_buffer[i]	 = ' ';
		tty_video_buffer[i + 1] = tty_color;
	}
	tty_set_cursor_offset(0);
}

void tty_init()
{
	tty_video_buffer = (u8*)TTY_BASE_ADDR;
	tty_color = TTY_DEFAULT_COLOR;
	tty_clear_screen();
}