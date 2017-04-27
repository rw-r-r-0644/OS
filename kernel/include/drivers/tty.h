#pragma once

#include <stdint.h>

#pragma pack(1)

typedef enum
{
	VGA_COLOR_BLACK			= 0x0,
	VGA_COLOR_BLUE			= 0x1,
	VGA_COLOR_GREEN			= 0x2,
	VGA_COLOR_CYAN			= 0x3,
	VGA_COLOR_RED			= 0x4,
	VGA_COLOR_MAGENTA		= 0x5,
	VGA_COLOR_BROWN			= 0x6,
	VGA_COLOR_LIGHT_GREY	= 0x7,
	VGA_COLOR_DARK_GREY		= 0x8,
	VGA_COLOR_LIGHT_BLUE	= 0x9,
	VGA_COLOR_LIGHT_GREEN	= 0xA,
	VGA_COLOR_LIGHT_CYAN	= 0xB,
	VGA_COLOR_LIGHT_RED		= 0xC,
	VGA_COLOR_LIGHT_MAGENTA	= 0xD,
	VGA_COLOR_LIGHT_BROWN	= 0xE,
	VGA_COLOR_WHITE			= 0xF,
} vga_color;

typedef struct
{
	vga_color bg : 4;
	vga_color fg : 4;
} vga_color_t;

#pragma pack(0)

//! Initialize the TTY text mode library
void tty_init();

//! Clear the screen to the current color
void tty_clear_screen();

//! Print a string to the current offset
void tty_print(char *s);

//! Print a char to the current offset
void tty_putchar(char c);

//! Print a custom color string
void tty_print_color(char *s, vga_color_t color);

//! Print a custom color char
void tty_putchar_color(char c, vga_color_t color);

//! Set the default text color
void tty_set_color(vga_color_t color);
