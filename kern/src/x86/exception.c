#include <stdio.h>
#include <x86/frame.h>
#include <kernel/panic.h>

static const char *exceptions_table[];

void exception_handler(struct interrupt_frame *frame)
{
	printf(
		"*** Exception %u (%s)\n"
		"*** Register dump:\n"
		"    eax:%08X ebx:%08X ecx:%08X edx:%08X esi:%08X edi:%08X\n"
		"    esp:%08X ebp:%08X eip:%08X eflags:%08X cs:%02X ss:%02X\n",
		frame->interrupt, exceptions_table[frame->interrupt],
		frame->eax, frame->ebx, frame->ecx, frame->edx, frame->esi, frame->edi,
		frame->esp, frame->ebp, frame->eip, frame->eflags, frame->cs, frame->ss
	);
	panic("Unhandled exception");
}

static const char *exceptions_table[] =
{
	"division by zero",
	"debug",
	"non maskable interrupt",
	"breakpoint",
	"overflow",
	"bound range exceeded",
	"invalid opcode",
	"device not available",
	"double fault",
	"coprocessor segment overrun",
	"invalid tss",
	"segment not present",
	"stack segment fault",
	"general protection fault",
	"page fault",
	"reserved",
	"x87 floating point exception",
	"alignment check",
	"machine check",
	"simd floating point exception",
	"virtualization exception",
	"reserved",
	"reserved",
	"reserved",
	"reserved",
	"reserved",
	"reserved",
	"reserved",
	"reserved",
	"reserved",
	"security exception",
	"reserved"
};
