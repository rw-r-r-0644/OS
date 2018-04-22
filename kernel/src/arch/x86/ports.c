#include <arch/x86/ports.h>

//! Read a byte from the specified port
u8 inb(u16 port)
{
	u8 result;
	asm("in %%dx, %%al" : "=a" (result) : "d" (port));
	return result;
}

//! Output a byte to the specified port
void outb(u16 port, u8 data)
{
	asm volatile("out %%al, %%dx" : : "a" (data), "d" (port));
}

//! Read a word from the specified port
u16 inw(u16 port)
{
	u16 result;
	asm("in %%dx, %%ax" : "=a" (result) : "d" (port));
	return result;
}

//! Output a word to the specified port
void outw(u16 port, u16 data)
{
	asm volatile("out %%ax, %%dx" : : "a" (data), "d" (port));
}

//! Read a long from the specified port
u32 inl(u16 port)
{
   u32 result;
   asm volatile("inl %%dx, %%eax" : "=a" (result) : "dN" (port));
   return result;
}

//! Output a long to the specified port
void outl(u16 port, u32 data)
{
   asm volatile("outl %%eax, %%dx" : : "d" (port), "a" (data));
}

//! Read a buffer from the specified port
void insl(u16 port, void *addr, u32 cnt)
{
	asm volatile("cld; rep insl" : "=D" (addr), "=c" (cnt) : "d" (port), "0" (addr), "1" (cnt) : "memory", "cc");
}

//! Output a buffer to the specified port
void outsl(u16 port, const void *addr, u32 cnt)
{
	asm volatile("cld; rep outsl" : "=S" (addr), "=c" (cnt) : "d" (port), "0" (addr), "1" (cnt) : "cc");
}