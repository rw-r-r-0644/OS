/*
 * utils/utils.h
 * Utility macros
 */

#pragma once

/* align macros (only for powers of 2) */
#define ALIGN_UP(addr, align)		(((addr) + (align - 1)) & -align)
#define ALIGN_DOWN(addr, align)		((addr) & -align)
