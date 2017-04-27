/*
 * gdt.h
 *
 * GDT functions
*/

#pragma once

//! Initializes unprotected GDT (declared in gdt.s)
void gdt_init();