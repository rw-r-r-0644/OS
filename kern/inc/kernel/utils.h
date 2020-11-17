#pragma once

#define _PACKED \
	__attribute__((packed))
#define _NORETURN \
	__attribute__((noreturn))
#define _ALIGNED(a) \
	__attribute__((aligned(a)))

#define _ALIGN2_DOWN(n, a) \
	((n) & -(a))
#define _ALIGN2_UP(n, a) \
	(((n) + ((a) - 1)) & -(a))

#define _DIVIDE2_DOWN(n, a) \
	((n) / (a))
#define _DIVIDE2_UP(n, a) \
	(((n) / (a)) + (((n) & ((a) - 1)) != 0))

#define _SHLDIV_DOWN(n, a) \
	((n) >> (a))
#define _SHLDIV_UP(n, a) \
	(((n) >> (a)) + (((n) & ((1 << (a)) - 1)) != 0))

#define _MIN(a, b) \
	(((a) < (b)) ? (a) : (b))
#define _MAX(a, b) \
	(((a) > (b)) ? (a) : (b))

