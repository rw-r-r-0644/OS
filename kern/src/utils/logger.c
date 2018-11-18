#include <utils/logger.h>
#include <dev/tty.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

// Colors contrassegnating levels
const uint16_t level_colors[] =
{
	vga_color(VGA_COLOR_BLACK, VGA_COLOR_WHITE),			// Debug
	vga_color(VGA_COLOR_BLACK, VGA_COLOR_WHITE),			// Info
	vga_color(VGA_COLOR_BLACK, VGA_COLOR_LIGHT_BROWN),	// Warning
	vga_color(VGA_COLOR_BLACK, VGA_COLOR_LIGHT_RED),		// Error
	vga_color(VGA_COLOR_BLACK, VGA_COLOR_RED)			// Fatal
};

// Character contrassegnating level
const char level_c[] =
{
	'D', // Debug
	'I', // Info
	'W', // Warning
	'E', // Error
	'F'  // Fatal
};

char log_buf[255];
void __log_printf(char * module_name, log_level_t level, char * fmt, ...)
{
	// Remove file path from module name
	char * p;
	for(;;)
	{
		p = strrchr(module_name, '/');
		if (p != NULL)
		{
			module_name = p;
		}
		else
		{
			module_name++;
			break;
		}
	}
	
	// Remove the dot
	if((p = strrchr(module_name, '.')) != NULL)
		*p = 0;
	
	// Do actual print
	printf("%c: %s: ", level_c[level], module_name);
	
	memset(log_buf, 0, 255);
	
	va_list va;
	va_start(va,fmt);
	vsnprintf(log_buf, 255, fmt, va);
	va_end(va);
	
	tty_print_color(log_buf, level_colors[level]);
	
	putchar('\n');
	tty_set_color(level_colors[0]);
}