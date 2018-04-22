/*
 * ports.h
 *
 * Wrappers for assembly I/O functions
*/


#pragma once

#include <stdint.h>

//! Read a byte from I/O
u8 inb(u16 port);

//! Read a word from I/O
u16 inw(u16 port);

//! Read a long from I/O
u32 inl(u16 port);

//! Read a buffer from I/O
void insl(u16 port, void *addr, u32 cnt);

//! Output a byte to I/O
void outb(u16 port, u8 data);

//! Output a word to I/O
void outw(u16 port, u16 data);

//! Output a long to I/O
void outl(u16 port, u32 data);

//! Outbut a buffer to I/O
void outsl(u16 port, const void *addr, u32 cnt);